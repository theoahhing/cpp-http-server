#pragma once
/*
    HTTP Parser file is designed to interpret a raw stream of bytes; breaking
    down unformatted HTTP messages into structured, usable data for applications
    such as web servers or browsers.
*/

#ifndef _HTTPPARSER_H
#define _HTTPPARSER_H

#include "Request.h"

namespace HttpParser
{

    enum class ParseState
    {
        RequestLine,
        Headers,
        Body,
        Complete,
        Error
    };

    enum class ParseResult
    {
        InProgress,
        Complete,
        Error
    };

    class Parser
    {
    public:
        Parser();
        ~Parser() = default;

        void appendData(const char* data, std::size_t size);
        ParseResult parse();

        bool isComplete() const;
        bool hasError() const;

        const Request& getRequest() const;

        void reset();

    private:
        bool parseRequestLine();
        bool parseHeaders();
        bool parseBody();

        // Utility helpers
        std::size_t findCRLF(const std::string& str, std::size_t start = 0) const;
        std::size_t findDoubleCRLF(const std::string& str) const;
        std::string normalizeHeaderKey(const std::string& key) const;

    private:
        ParseState state;

        std::string buffer;
        Request request;

        std::size_t contentLength;
        std::size_t parsedBytes;

        bool errorFlag;
    };

}

#endif // _HTTPPARSER_H