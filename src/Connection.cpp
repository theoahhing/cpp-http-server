#include "../include/Connection.h"

/*=====================================================================================

    CONSTRUCTOR:
        Sets all members to safe defaults.

======================================================================================*/

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

/*=====================================================================================

    DESTRUCTOR:
        Stop connection.

======================================================================================*/

Connection::~Connection(void)
{
    stop();
}

/*=====================================================================================

    READ BYTES FUNCTION:
        Read the bytes.

======================================================================================*/

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

/*=====================================================================================

    WRITE BYTES FUNCTION:
        Write bytes.

======================================================================================*/

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

/*=====================================================================================

    STOP FUNCTION:
        Stop connection.

======================================================================================*/

void Connection::stop(void)
{
    if (clientSocket == INVALID_SOCKET_HANDLE) {
        isConnected = false;
        return;
    }

    isConnected = false;
    closeClientSocket();
}

/*=====================================================================================

    IS VALID FUNCTION:
        Checks to see whether socket handle is valid.

======================================================================================*/

bool Connection::isValid(void) const
{
    return (clientSocket != INVALID_SOCKET_HANDLE);
}

/*=====================================================================================

    IS ACTIVE FUNCTION:
        Checks to see whether connection is active.

======================================================================================*/

bool Connection::isActive(void) const
{
    return isConnected;
}

/*=====================================================================================

    GET HANDLE FUNCTION:
        Returns the client socket handle.

======================================================================================*/

SocketHandle Connection::getHandle(void) const
{
    return clientSocket;
}

/*=====================================================================================

    GET IP ADDRESS FUNCTION:
        Returns the IP Address.

======================================================================================*/

std::string Connection::getIpAddress(void) const
{
    return ipAddress;
}

/*=====================================================================================

    GET PORT FUNCTION:
        Returns the port.

======================================================================================*/

unsigned short Connection::getPort(void) const
{
    return port;
}

/*=====================================================================================

    CLOSE CLIENT SOCKET FUNCTION:
        Close the socket.

======================================================================================*/

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