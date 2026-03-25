#pragma once

#ifndef _CONNECTION_H
#define _CONNECTION_H

#include "SocketTypes.h"
#include "CommonIncludes.h"
#include "HttpParser.h"

class Connection
{
public:
    Connection(SocketHandle socketHandle, const sockaddr_in& address);
    ~Connection(void);

    int readBytes(char* buffer, int length);
    int writeBytes(const char* data, int length);

    void stop(void);

    bool isValid(void) const;
    bool isActive(void) const;

    SocketHandle getHandle(void) const;
    std::string getIpAddress(void) const;
    unsigned short getPort(void) const;

private:
    void closeClientSocket(void);

private:
    SocketHandle clientSocket;
    sockaddr_in clientAddress;

    std::string ipAddress;
    unsigned short port;

    bool isConnected;
};

#endif