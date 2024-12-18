#include <iostream>
#include <thread>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

void receive_messages(int client_socket)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0)
        {
            cout << buffer << endl;
        }
    }
}

int main()
{
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        cerr << "Socket creation failed." << endl;
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "Connection failed." << endl;
        return -1;
    }

    cout << "Enter your username: ";
    string username;
    getline(cin, username);
    send(client_socket, username.c_str(), username.length(), 0);

    thread recv_thread(receive_messages, client_socket);
    recv_thread.detach();

    while (true)
    {
        string message;
        getline(cin, message);
        send(client_socket, message.c_str(), message.length(), 0);
    }

    close(client_socket);
    return 0;
}
