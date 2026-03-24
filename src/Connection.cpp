/*
	This Connection file has the following responsibilities:
		- Store client socket handle
		- Read incoming bytes
		- Write outgoing bytes
		- close socket
*/

#include "../include/Connection.h"

Connection::Connection(SocketHandle socketHandle, const sockaddr_in& address)
    : clientSocket(socketHandle),
    clientAddress(address),
    ipAddress(""),
    port(0),
    isConnected(false)
{
    char ipBuffer[INET_ADDRSTRLEN] = { 0 };

    if (inet_ntop(AF_INET, &(clientAddress.sin_addr), ipBuffer, INET_ADDRSTRLEN) != nullptr) {
        ipAddress = ipBuffer;
    }
    else {
        ipAddress = "Unknown";
    }

    port = ntohs(clientAddress.sin_port);

    isConnected = (clientSocket != INVALID_SOCKET_HANDLE);
}

Connection::~Connection(void)
{
    stop();
}

int Connection::readBytes(char* buffer, int length)
{
    if (buffer == nullptr || length <= 0 || clientSocket == INVALID_SOCKET_HANDLE) {
        return -1;
    }

    int bytesReceived = recv(clientSocket, buffer, length, 0);

    if (bytesReceived <= 0) {
        isConnected = false;
    }

    return bytesReceived;
}

int Connection::writeBytes(const char* data, int length)
{
    if (data == nullptr || length <= 0 || clientSocket == INVALID_SOCKET_HANDLE) {
        return -1;
    }

    int bytesSent = send(clientSocket, data, length, 0);

    if (bytesSent < 0) {
        isConnected = false;
    }

    return bytesSent;
}

void Connection::stop(void)
{
    if (clientSocket == INVALID_SOCKET_HANDLE) {
        isConnected = false;
        return;
    }

    isConnected = false;
    closeClientSocket();
}

bool Connection::isValid(void) const
{
    return (clientSocket != INVALID_SOCKET_HANDLE);
}

bool Connection::isActive(void) const
{
    return isConnected;
}

SocketHandle Connection::getHandle(void) const
{
    return clientSocket;
}

std::string Connection::getIpAddress(void) const
{
    return ipAddress;
}

unsigned short Connection::getPort(void) const
{
    return port;
}

void Connection::closeClientSocket(void)
{
    if (clientSocket == INVALID_SOCKET_HANDLE) {
        return;
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif

    clientSocket = INVALID_SOCKET_HANDLE;
}