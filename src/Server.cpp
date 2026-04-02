#include "../include/Server.h"

/*=====================================================================================

    CONSTRUCTOR:
        Sets all members to safe defaults.

======================================================================================*/

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

/*=====================================================================================

    DESTRUCTOR:
        No matter how the server exits, clean everything up.

======================================================================================*/

Server::~Server(void)
{
    stop();
}

/*=====================================================================================

    START FUNCTION:
        Step-by-step setup with rollback if anything fails. Start function is to:
            1. Initialise platform.
            2. Create & Configure Sockets.
            3. Bind & Listen.
            4. Start accept loop.

======================================================================================*/

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

/*=====================================================================================

    STOP FUNCTION:
        Shut everything down if it exists. Stop function is to:
            1. Stop loop.
            2. Close listening socket.
            3. Clean up Winsock.

======================================================================================*/

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

/*=====================================================================================

    IS RUNNING FUNCTION:
        Gets & returns the 'running' status.

======================================================================================*/

bool Server::isRunning(void) const
{
    return running;
}

/*=====================================================================================

    INIT PLATFORM FUNCTION:
        Prepare OS-specific networking only once.

======================================================================================*/

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

/*=====================================================================================

    CLEANUP PLATFORM FUNCTION:
        Undo platform setup when no longer needed.

======================================================================================*/

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

/*=====================================================================================

    CREATE LISTENING SOCKET FUNCTION:
        Creates the main endpoint for incoming connections.

======================================================================================*/

bool Server::createListeningSocket(void)
{
    listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listeningSocket == INVALID_SOCKET_HANDLE) {
        std::cerr << "Failed to create listening socket.\n";
        return false;
    }

    return true;
}

/*=====================================================================================

    CONFIGURE SOCKET FUNCTION:
        Adjust socket behaviour before using it.

======================================================================================*/

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

#ifdef _WIN32
    if (result == SOCKET_ERROR) {
#else
    if (result < 0) {
#endif
        std::cerr << "Failed to set socket options.\n";
        return false;
    }

    return true;
    }

/*=====================================================================================

    BIND SOCKET FUNCTION:
        Attach the socket to a specific IP Address and Port.

======================================================================================*/

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

#ifdef _WIN32
    if (result == SOCKET_ERROR) {
#else
    if (result < 0) {
#endif
        std::cerr << "Bind failed.\n";
        return false;
    }

    return true;
    }

/*=====================================================================================

    START LISTENING FUNCTION:
        Tell the OS to queue incoming connections.

======================================================================================*/

bool Server::startListening(void)
{
    int result = listen(listeningSocket, backlog);

#ifdef _WIN32
    if (result == SOCKET_ERROR) {
#else
    if (result < 0) {
#endif
        std::cerr << "Listen failed.\n";
        return false;
    }

    return true;
    }

/*=====================================================================================

    RUN ACCEPT LOOP FUNCTION:
        Continuously accept and handle clients.

======================================================================================*/

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

        handleClient(clientSocket, clientAddress);
    }
}

/*=====================================================================================

    HANDLE CLIENT FUNCTION:
        Read raw bytes, parse request, generate response, send response, close client.

======================================================================================*/

void Server::handleClient(SocketHandle clientSocket, const sockaddr_in & clientAddress)
{
    char ipBuffer[INET_ADDRSTRLEN] = { 0 };

    if (inet_ntop(AF_INET, &(clientAddress.sin_addr), ipBuffer, INET_ADDRSTRLEN) == nullptr) {
        std::snprintf(ipBuffer, sizeof(ipBuffer), "%s", "Unknown");
    }

    std::cout << "Accepted connection from "
        << ipBuffer
        << ":"
        << ntohs(clientAddress.sin_port)
        << "\n";

    Connection connection(clientSocket, clientAddress);
    HttpParser::Parser parser;

    char buffer[config::BUFFER_SIZE] = { 0 };

    while (connection.isActive()) {
        int bytesReceived = connection.readBytes(buffer, config::BUFFER_SIZE);

        if (bytesReceived <= 0) {
            break;
        }

        std::cout << "Received " << bytesReceived << " bytes:\n";
        std::cout.write(buffer, bytesReceived);
        std::cout << "\n\n";

        parser.appendData(buffer, static_cast<std::size_t>(bytesReceived));

        HttpParser::ParseResult result = parser.parse();

        if (result == HttpParser::ParseResult::InProgress) {
            std::memset(buffer, 0, sizeof(buffer));
            continue;
        }

        Response response;

        if (result == HttpParser::ParseResult::Error) {
            response = makeBadRequestResponse();
        }
        else {
            response = handleRequest(parser.getRequest());
        }

        const std::string rawResponse = response.toString();

        int bytesSent = connection.writeBytes(rawResponse.c_str(),
            static_cast<int>(rawResponse.size()));

        if (bytesSent <= 0) {
            break;
        }

        break; // simple version: one request per connection
    }

    connection.stop();

    std::cout << "Connection closed: "
        << connection.getIpAddress()
        << ":"
        << connection.getPort()
        << "\n";
}

/*=====================================================================================

    HANDLE REQUEST FUNCTION:
        Decide what response to send back based on parsed HTTP request.

======================================================================================*/

Response Server::handleRequest(const Request & request)
{
    Response response;

    if (request.method != "GET") {
        response.setStatus(405, "Method Not Allowed");
        response.setHeader("Content-Type", "text/html");
        response.body =
            "<html><body><h1>405 Method Not Allowed</h1></body></html>";
        return response;
    }

    if (request.path == "/") {
        response.setStatus(200, "OK");
        response.setHeader("Content-Type", "text/html");
        response.body =
            "<html>"
            "<head><title>My HTTP Server</title></head>"
            "<body><h1>Hello from your C++ HTTP Server</h1></body>"
            "</html>";
        return response;
    }

    return makeNotFoundResponse();
}

/*=====================================================================================

    BAD REQUEST RESPONSE FUNCTION:
        Build a simple 400 response.

======================================================================================*/

Response Server::makeBadRequestResponse(void) const
{
    Response response;
    response.setStatus(400, "Bad Request");
    response.setHeader("Content-Type", "text/html");
    response.body =
        "<html><body><h1>400 Bad Request</h1></body></html>";
    return response;
}

/*=====================================================================================

    NOT FOUND RESPONSE FUNCTION:
        Build a simple 404 response.

======================================================================================*/

Response Server::makeNotFoundResponse(void) const
{
    Response response;
    response.setStatus(404, "Not Found");
    response.setHeader("Content-Type", "text/html");
    response.body =
        "<html><body><h1>404 Not Found</h1></body></html>";
    return response;
}

/*=====================================================================================

    CLOSE SOCKET HANDLE FUNCTION:
        Safely close sockets across platforms.

======================================================================================*/

void Server::closeSocketHandle(SocketHandle & socketHandle)
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