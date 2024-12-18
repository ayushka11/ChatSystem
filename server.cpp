#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <mutex>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <algorithm>
using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

mutex log_mutex;
mutex client_mutex;

map<int, string> clients;
vector<int> client_fds;

void log_message(const string &message)
{
    lock_guard<mutex> lock(log_mutex);
    ofstream log_file("./chat_log.txt", ios::app);
    if (log_file.is_open())
    {

        log_file << message << endl;
        log_file.close();
    }
    else
    {
        cerr << "Failed to open log file." << endl;
    }
}

void send_message_to_client(int recipient_fd, const string &message)
{
    send(recipient_fd, message.c_str(), message.length(), 0);
}

void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE];
    string welcome_msg = "Welcome to the chat! Send messages as '@username: message'\n";
    send(client_socket, welcome_msg.c_str(), welcome_msg.length(), 0);

    string username;

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    username = string(buffer);
    {
        lock_guard<mutex> lock(client_mutex);
        clients[client_socket] = username;
        client_fds.push_back(client_socket);
    }
    cout << username << " connected!" << endl;

    string log_entry = username + " has joined the chat.";
    log_message(log_entry);

    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (bytes_received <= 0)
        {
            cout << username << " disconnected." << endl;
            log_message(username + " disconnected.");
            break;
        }

        string message(buffer);
        cout << "[" << username << "]: " << message << endl;

        log_message("[" + username + "]: " + message);

        if (message.find("@") == 0)
        {
            size_t colon_pos = message.find(":");
            if (colon_pos != string::npos)
            {
                string recipient = message.substr(1, colon_pos - 1);
                string actual_message = message.substr(colon_pos + 1);

                lock_guard<mutex> lock(client_mutex);
                for (auto &client : clients)
                {
                    if (client.second == recipient)
                    {
                        string full_message = "[DM from " + username + "]: " + actual_message;
                        send_message_to_client(client.first, full_message);
                        break;
                    }
                }
            }
        }
    }

    close(client_socket);
    {
        lock_guard<mutex> lock(client_mutex);
        clients.erase(client_socket);
        client_fds.erase(remove(client_fds.begin(), client_fds.end(), client_socket), client_fds.end());
    }
}

int main()
{
    int server_socket;
    struct sockaddr_in server_addr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        cerr << "Failed to create socket." << endl;
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "Bind failed." << endl;
        return -1;
    }

    if (listen(server_socket, 5) < 0)
    {
        cerr << "Listen failed." << endl;
        return -1;
    }

    cout << "Server is listening on port " << PORT << "..." << endl;

    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);

        if (client_socket < 0)
        {
            cerr << "Accept failed." << endl;
            continue;
        }

        thread client_thread(handle_client, client_socket);
        client_thread.detach();
    }

    close(server_socket);
    return 0;
}
