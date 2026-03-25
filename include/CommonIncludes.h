#pragma once

/*
    Please note that this header file only includes common header files; only add to this
    file if header file for repetitive includes across multiple files. It is suggested to
    locally include header files for one-off includes for optimisation.

    Also, this is just a template so add or remove based on project requirements.
*/

#ifndef _COMMONINCLUDES_H
#define _COMMONINCLUDES_H

/*
    header files removed from original:
        - #include <thread>
        - #include <future>
        - #include <map>
        - #include <mutex>
        - #include <condition_variable>
        - #include <chrono>
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <istream>
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>
#include <memory>
#include <ctime>
#include <numeric>
#include <utility>
#include <cerrno>


// ================================
// Linux Abstraction
// ================================

#ifdef __linux__
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <errno.h>
#endif

// ================================
// Type Aliases
// ================================

using byte          = uint8_t;
using leastByte     = uint_least8_t;
using fastByte      = uint_fast8_t;
using word          = uint16_t;
using leastWord     = uint_least16_t;
using fastWord      = uint_fast16_t;
using dword         = uint32_t;
using leastDWord    = uint_least32_t;
using fastDWord     = uint_fast32_t;
using qword         = uint64_t;
using leastQWord    = uint_least64_t;
using fastQWord     = uint_fast64_t;


// ================================
// Global Config
// ================================

namespace config {
    constexpr int DEFAULT_PORT = 8080;
    constexpr int DEFAULT_BACKLOG = 10;
    constexpr int BUFFER_SIZE = 4096;
    constexpr const char* DEFAULT_IP_ADDRESS = "127.0.0.1";
}

// ================================
// Utility Helpers
// ================================

namespace util {
    inline std::string getLastError() {
#ifdef _WIN32
        return std::to_string(WSAGetLastError());
#else
        return std::strerror(errno);
#endif
    }
}



#endif