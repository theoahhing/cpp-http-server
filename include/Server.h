#pragma once
/*
    Try Config -> Properties -> Linker -> Input -> Additional Dependendencies and add: Ws2_32.lib
    Once configured, initialize using WSAStartup() and WSACleanup() at beginning and end
    of program.

    Or g++ main.cpp -o main.exe -lws2_32

    Server-side socket programming stages:
        1. Create server socket.
        2. Define server address.
        3. Bind socket to address.
        4. Listen for incoming connections.
        5. Accept client connection.
        6. Receive data from client.
        7. Close server socket.
    Client-side socket programming stages:
        1. Create client socket.
        2. Define server address.
        3. Connect to server.
        4. Send Data to the server.
        5. Close client socket.

    Server class to use following steps:
        1. Constructor -> Set serverAddress & initialize class variables
        2. Start:
            2.1: Initiate Platform
            2.2: Create Listening Socket
            2.3: Configure Socket Options
            2.4: Bind Socket
            2.5: Start Listening
            2.6: Run Accept Loop
        3. Initiate -> Specify version of Windows Sockets
        4. Create Listening Socket -> Create Server Socket
        5. Configure Socket:
            5.1: Choose Socket to configure
            5.2: Define level at which the option is defined
            5.3: Select option for which value is to be set
            5.4: Point to buffer
            5.5: Byte size of the buffer pointed to
        6. Bind Socket:
            6.1: Convert from host byte to TCP network byte.
                6.1.1: Machines can be little-endian or big-endian.
            6.2: Convert IP Address from text to binary format.
            6.3: Bind:
                6.3.1: File Descriptor of socket.
                6.3.2: Pointer to sockaddr structure containing local IP and Port.
                6.3.3: Size of address structure.
        7. Start Listening -> listen to socket fd and define max no. of pending connections queued.
        8. Accept request and loop.

*/

#ifndef _SERVER_H
#define _SERVER_H

#include "Connection.h"
#include "Response.h"

class Server
{
public:
    Server(
        int port = config::DEFAULT_PORT,
        const std::string& ipAddress = config::DEFAULT_IP_ADDRESS,
        int backlog = config::DEFAULT_BACKLOG);

    ~Server(void);

    bool start(void);
    void stop(void);

    bool isRunning(void) const;

private:
    bool initPlatform(void);
    void cleanupPlatform(void);

    bool createListeningSocket(void);
    bool configureSocketOptions(void);
    bool bindSocket(void);
    bool startListening(void);

    void runAcceptLoop(void);
    void handleClient(SocketHandle clientSocket, const sockaddr_in& clientAddress);

    Response handleRequest(const Request& request);
    Response makeBadRequestResponse(void) const;
    Response makeNotFoundResponse(void) const;

    void closeSocketHandle(SocketHandle& socketHandle);

private:
    SocketHandle listeningSocket;
    sockaddr_in serverAddress;

    int port;
    int backlog;
    std::string ipAddress;

    bool running;
    bool platformInitialized;
};


#endif