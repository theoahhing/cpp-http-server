#pragma once

#ifndef SOCKETTYPES_H
#define SOCKETTYPES_H

#if defined(__linux__) || defined(__unix__)

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#define CLOSE_SOCKET(s) ::close(s)

using SocketHandle = int;
constexpr SocketHandle INVALID_SOCKET_HANDLE = -1;

#elif defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#define CLOSE_SOCKET(s) ::closesocket(s)

using SocketHandle = SOCKET; // #define SOCKET SocketHandle
constexpr SocketHandle INVALID_SOCKET_HANDLE = INVALID_SOCKET;
#endif

#endif // SOCKETTYPES_H