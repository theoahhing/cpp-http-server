#pragma once

#ifndef _HTTPPARSER_H
#define _HTTPPARSER_H

#include "CommonIncludes.h"
#include "Request.h"

namespace HttpParser
{
    // ================================
    // Parser State Machine
    // ================================

    enum class ParseState
    {
        RequestLine,
        Headers,
        Body,
        Complete,
        Error
    };

    // ================================
    // Parse Result (Optional but useful)
    // ================================

    enum class ParseResult
    {
        InProgress,
        Complete,
        Error
    };

    // ================================
    // HTTP Parser Class
    // ================================

    class Parser
    {
    public:
        Parser();
        ~Parser() = default;

        // Append raw network bytes into parser buffer
        void appendData(const char* data, std::size_t size);

        // Run parsing logic (state machine)
        ParseResult parse();

        // Query State
        bool isComplete() const;
        bool hasError() const;


        // Access parsed request
        const Request& getRequest() const;

        // Reset parser for next request (import for keep-alive)
        void reset();

    private:
        // ================================
        // Internal Parsing Helpers
        // ================================
        bool parseRequestLine();
        bool parseHeaders();
        bool parseBody();

        // Utility helpers
        std::size_t findCRLF(const std::string& str, std::size_t start = 0) const;
        std::size_t findDoubleCRLF(const std::string& str) const;

    private:
        // ================================
        // Internal State
        // ================================
        ParseState state;

        std::string buffer;        // Accumulated raw bytes
        Request request;           // Parsed request

        std::size_t contentLength; // Expected body size
        std::size_t parsedBytes;   // Bytes already consumed

        bool errorFlag;

    };

}

#endif // _HTTPPARSER_H