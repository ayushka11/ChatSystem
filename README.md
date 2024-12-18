# ChatSystem
Real-Time Chat System with Socket Programming in C++

## Description
This project implements a **multi-user real-time chat system** using C++ sockets. The system supports:
- Private messaging (DMs) between users.
- Logging all chat messages to a `chat_log.txt` file for historical reference.
- Thread-safe logging using **mutex** and file I/O.

The system consists of a **server** and multiple **clients** that can connect and communicate simultaneously.

---

## Features
- **Multi-User Support**: Server can handle multiple clients.
- **Private Messaging**: Users can send DMs to each other.
- **Thread-Safe Chat Logging**: All messages are logged to a file in real-time.
- **C++ STL Usage**: Makes use of STL components like `vector`, `string`, and `mutex`.

---

## How to Run

### 1. **Create the chat log file**

Create a file chat_log.txt by using below command:

```bash
touch chat_log.txt
```

### 2. **Build the Server and Client**

Run the following commands in your terminal to compile the program:

```bash
# Compile the server
g++ -o build/server server.cpp -std=c++17 -pthread

# Compile the client
g++ -o build/client client.cpp -std=c++17 -pthread
```

### 3. **Run the server**

Start the server first:

```bash
./build/server
```

### 4. **Run the clients**

Open multiple terminals and run the client program:

```bash
./build/client
```

You can now send messages between clients through the server.

