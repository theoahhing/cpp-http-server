
#include "../include/Server.h"

Server::Server(int port, const std::string& ipAddress, int backlog)
    : listeningSocket(INVALID_SOCKET_HANDLE),
    port(port),
    backlog(backlog),
    ipAddress(ipAddress),
    running(false),
    platformInitialized(false)
{
    std::memset(&serverAddress, 0, sizeof(serverAddress));
}

Server::~Server(void)
{
    stop();
}

bool Server::start(void)
{
    if (running) {
        std::cout << "Server is already running.\n";
        return true;
    }

    if (!initPlatform()) {
        return false;
    }

    if (!createListeningSocket()) {
        cleanupPlatform();
        return false;
    }

    if (!configureSocketOptions()) {
        closeSocketHandle(listeningSocket);
        cleanupPlatform();
        return false;
    }

    if (!bindSocket()) {
        closeSocketHandle(listeningSocket);
        cleanupPlatform();
        return false;
    }

    if (!startListening()) {
        closeSocketHandle(listeningSocket);
        cleanupPlatform();
        return false;
    }

    running = true;

    std::cout << "Server started on " << ipAddress << ":" << port << "\n";

    runAcceptLoop();

    return true;
}

void Server::stop(void)
{
    if (!running && listeningSocket == INVALID_SOCKET_HANDLE && !platformInitialized) {
        return;
    }

    running = false;

    closeSocketHandle(listeningSocket);
    cleanupPlatform();

    std::cout << "Server stopped.\n";
}

bool Server::isRunning(void) const
{
    return running;
}

bool Server::initPlatform(void)
{
#ifdef _WIN32
    if (platformInitialized) {
        return true;
    }

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << "\n";
        return false;
    }

    platformInitialized = true;
#else
    platformInitialized = true;
#endif

    return true;
}

void Server::cleanupPlatform(void)
{
#ifdef _WIN32
    if (platformInitialized) {
        WSACleanup();
        platformInitialized = false;
    }
#else
    platformInitialized = false;
#endif
}

bool Server::createListeningSocket(void)
{
    listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listeningSocket == INVALID_SOCKET_HANDLE) {
        std::cerr << "Failed to create listening socket.\n";
        return false;
    }

    return true;
}

bool Server::configureSocketOptions(void)
{
    int optionValue = 1;

#ifdef _WIN32
    const char* optionPointer = reinterpret_cast<const char*>(&optionValue);
#else
    const void* optionPointer = &optionValue;
#endif

    int result = setsockopt(
        listeningSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        optionPointer,
        sizeof(optionValue)
    );

    if (result < 0) {
        std::cerr << "Failed to set socket options.\n";
        return false;
    }

    return true;
}

bool Server::bindSocket(void)
{
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(static_cast<unsigned short>(port));

    int convertResult = inet_pton(AF_INET, ipAddress.c_str(), &serverAddress.sin_addr);
    if (convertResult <= 0) {
        std::cerr << "Invalid IP address: " << ipAddress << "\n";
        return false;
    }

    int result = bind(
        listeningSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    if (result < 0) {
        std::cerr << "Bind failed.\n";
        return false;
    }

    return true;
}

bool Server::startListening(void)
{
    int result = listen(listeningSocket, backlog);

    if (result < 0) {
        std::cerr << "Listen failed.\n";
        return false;
    }

    return true;
}

void Server::runAcceptLoop(void)
{
    while (running) {
        sockaddr_in clientAddress;
        std::memset(&clientAddress, 0, sizeof(clientAddress));

#ifdef _WIN32
        int clientLength = sizeof(clientAddress);
#else
        socklen_t clientLength = sizeof(clientAddress);
#endif

        SocketHandle clientSocket = accept(
            listeningSocket,
            reinterpret_cast<sockaddr*>(&clientAddress),
            &clientLength
        );

        if (clientSocket == INVALID_SOCKET_HANDLE) {
            if (running) {
                std::cerr << "Accept failed.\n";
            }
            continue;
        }

        char ipBuffer[INET_ADDRSTRLEN] = { 0 };
        if (inet_ntop(AF_INET, &(clientAddress.sin_addr), ipBuffer, INET_ADDRSTRLEN) == nullptr) {
            std::strcpy(ipBuffer, "Unknown");
        }

        std::cout << "Accepted connection from "
            << ipBuffer
            << ":"
            << ntohs(clientAddress.sin_port)
            << "\n";

        Connection connection(clientSocket, clientAddress);

        char buffer[config::BUFFER_SIZE] = { 0 };

        while (connection.isActive()) {
            int bytesReceived = connection.readBytes(buffer, config::BUFFER_SIZE);

            if (bytesReceived <= 0) {
                break;
            }

            int bytesSent = connection.writeBytes(buffer, bytesReceived);

            if (bytesSent <= 0) {
                break;
            }

            std::memset(buffer, 0, sizeof(buffer));
        }

        connection.stop();

        std::cout << "Connection closed: "
            << connection.getIpAddress()
            << ":"
            << connection.getPort()
            << "\n";
    }
}

void Server::closeSocketHandle(SocketHandle& socketHandle)
{
    if (socketHandle == INVALID_SOCKET_HANDLE) {
        return;
    }

#ifdef _WIN32
    closesocket(socketHandle);
#else
    close(socketHandle);
#endif

    socketHandle = INVALID_SOCKET_HANDLE;
}