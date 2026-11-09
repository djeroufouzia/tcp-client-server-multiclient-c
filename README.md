# TCP Client-Server Communication System with Multiple Clients in C

## Project Overview

This project implements a multi-client communication system using TCP sockets in C.

The application follows a client-server architecture where a single server accepts and manages multiple client connections simultaneously.

The server assigns a unique ID to each connected client and uses POSIX threads (`pthread`) to handle incoming messages independently.

Clients can send messages to the server, while the server can reply individually to a specific client using its assigned ID.

**Architecture:** One Server – Multiple Clients.

## Features

- TCP communication using IPv4 sockets.
- Multiple simultaneous client connections.
- Maximum of 10 connected clients.
- Unique ID assignment for each client.
- Individual client identification using IP address and port.
- Concurrent client handling using POSIX threads.
- Individual server-to-client messaging.
- Simultaneous sending and receiving on the client side.
- Client disconnection using the `quit` command.
- Server shutdown using the `quit` command.

## Technologies

- C Programming Language
- TCP/IP
- IPv4
- POSIX Sockets API
- POSIX Threads (`pthread`)
- GCC
- Linux

## Compilation

Compile the server:

```bash
gcc server.c -o server -pthread
```

Compile the client:

```bash
gcc client.c -o client -pthread
```

## Execution

### 1. Start the Server

On the server machine:

```bash
./server
```

The server listens on port `8084` and waits for incoming client connections.

### 2. Configure the Client

Before running the client, update the server IP address in `client.c`:

```c
#define SERVER_IP "192.168.56.7"
```

Replace the IP address with the actual IP address of your server.

### 3. Start the Clients

On each client terminal:

```bash
./client
```

Multiple clients can connect to the same server, up to the maximum limit of 10 clients.

### 4. Client-to-Server Communication

Clients can type messages and send them to the server.

The server displays each message with the client's ID, IP address, and port.

### 5. Server-to-Client Communication

To send a message to a specific client, use the following format in the server terminal:

```text
CLIENT_ID message
```

Example:

```text
1 Hello Client 1
```

This sends a message to the client assigned ID 1.

### 6. Disconnect

To disconnect a client, type:

```text
quit
```

To stop the server, type:

```text
quit
```

## Architecture

### Server Side

The server performs the following operations:

1. Creates a TCP socket using `socket()`.
2. Configures the socket using `setsockopt()`.
3. Assigns an IP address and port using `bind()`.
4. Waits for incoming connections using `listen()`.
5. Accepts client connections using `accept()`.
6. Assigns an ID to each connected client.
7. Stores client socket descriptors and network addresses.
8. Creates a separate receiving thread for each client.
9. Receives messages using `recv()`.
10. Sends individual replies using `send()`.

### Client Side

The client performs the following operations:

1. Creates a TCP socket using `socket()`.
2. Converts the server IP address using `inet_pton()`.
3. Establishes a connection using `connect()`.
4. Creates a receiving thread using `pthread_create()`.
5. Reads user input from the terminal.
6. Sends messages using `send()`.
7. Receives server messages using `recv()`.

The receiving thread allows the client to receive messages while the main thread is waiting for user input.

## Learning Objectives

- Understand TCP socket programming in C.
- Understand connection-oriented communication.
- Implement a multi-client server.
- Manage multiple client connections.
- Understand the TCP socket lifecycle.
- Work with `socket()`, `bind()`, `listen()`, `accept()`, and `connect()`.
- Use `send()` and `recv()` for data transmission.
- Understand concurrency using POSIX threads.
- Manage client socket descriptors and network addresses.
- Understand the differences between TCP and UDP.

## Future Improvements

- Add mutexes to protect shared client information.
- Implement broadcast messaging.
- Add client usernames.
- Improve thread synchronization and shutdown handling.
- Handle partial sends and TCP message framing.
- Improve error handling and connection management.
- Add authentication and encrypted communication.
