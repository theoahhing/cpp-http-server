#pragma once

#ifndef _SERVER_H
#define _SERVER_H

#include "Connection.h"

/*
	Try Config -> Properties -> Linker -> Input -> Additional Dependendencies and add: Ws2_32.lib
	Once configured, initialize using WSAStartup() and WSACleanup() at beginning and end
	of program.

	Or g++ main.cpp -o main.exe -lws2_32

*/

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