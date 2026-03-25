#include "../include/HttpParser.h"

namespace HttpParser
{
    // ================================
    // Constructor
    // ================================

    Parser::Parser()
        : state(ParseState::RequestLine),
          buffer(""),
          request(),
          contentLength(0),
          parsedBytes(0),
          errorFlag(false)
    {

    }

    // ================================
    // Public Methods
    // ================================

    void Parser::appendData(const char* data, std::size_t size)
    {
        if (data == nullptr || size == 0) {
            return;
        }

        buffer.append(data, size);
    }

    ParseResult Parser::parse()
    {
        if (errorFlag) {
            state = ParseState::Error;
            return ParseResult::Error;
        }

        bool madeProgress = true;

        while (madeProgress) {
            madeProgress = false;

            switch (state) {
            case ParseState::RequestLine:
                if (parseRequestLine()) {
                    madeProgress = true;
                }
                break;

            case ParseState::Headers:
                if (parseHeaders()) {
                    madeProgress = true;
                }
                break;

            case ParseState::Body:
                if (parseBody()) {
                    madeProgress = true;
                }
                break;

            case ParseState::Complete:
                return ParseResult::Complete;

            case ParseState::Error:
                return ParseResult::Error;

            default:
                errorFlag = true;
                state = ParseState::Error;
                return ParseResult::Error;
            }
        }

        if (state == ParseState::Complete) {
            return ParseResult::Complete;
        }

        if (state == ParseState::Error || errorFlag) {
            return ParseResult::Error;
        }

        return ParseResult::InProgress;
    }

    bool Parser::isComplete() const
    {
        return state == ParseState::Complete;
    }

    bool Parser::hasError() const
    {
        return state == ParseState::Error || errorFlag;
    }

    const Request& Parser::getRequest() const
    {
        return request;
    }

    void Parser::reset()
    {
        state = ParseState::RequestLine;
        buffer.clear();
        request = Request{};
        contentLength = 0;
        parsedBytes = 0;
        errorFlag = false;
    }

    // ================================
    // Private Parsing Helpers
    // ================================
    bool Parser::parseRequestLine()
    {
        const std::size_t lineEnd = findCRLF(buffer, parsedBytes);

        if (lineEnd == std::string::npos) {
            return false;
        }

        const std::string line = buffer.substr(parsedBytes, lineEnd - parsedBytes);
        parsedBytes = lineEnd + 2; // Skip \r\n

        std::istringstream iss(line);

        if (!(iss >> request.method >> request.path >> request.version)) {
            errorFlag = true;
            state = ParseState::Error;
            return false;
        }

        // Basic validation
        if (request.method.empty() || request.path.empty() || request.version.empty()) {
            errorFlag = true;
            state = ParseState::Error;
            return false;
        }

        // Optional: basic HTTP version sanity check
        if (request.version.rfind("HTTP/", 0) != 0) {
            errorFlag = true;
            state = ParseState::Error;
            return false;
        }

        state = ParseState::Headers;
        return true;
    }

    bool Parser::parseHeaders()
    {
        while (true) {
            const std::size_t lineEnd = findCRLF(buffer, parsedBytes);

            if (lineEnd == std::string::npos) {
                return false;
            }

            // Empty line means end of headers
            if (lineEnd == parsedBytes) {
                parsedBytes += 2; // Skip final \r\n

                auto it = request.headers.find("Content-Length");
                if (it != request.headers.end()) {
                    try {
                        contentLength = static_cast<std::size_t>(std::stoul(it->second));
                    }
                    catch (...) {
                        errorFlag = true;
                        state = ParseState::Error;
                        return false;
                    }
                }
                else {
                    contentLength = 0;
                }

                if (contentLength > 0) {
                    state = ParseState::Body;
                }
                else {
                    state = ParseState::Complete;
                }

                return true;
            }

            const std::string line = buffer.substr(parsedBytes, lineEnd - parsedBytes);
            parsedBytes = lineEnd + 2; // Skip \r\n

            const std::size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                errorFlag = true;
                state = ParseState::Error;
                return false;
            }

            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            // Trim leading whitespace from header value
            value.erase(value.begin(),
                std::find_if(value.begin(), value.end(),
                    [](unsigned char ch) { return !std::isspace(ch); }));

            // Trim trailing whitespace from header value
            value.erase(std::find_if(value.rbegin(), value.rend(),
                [](unsigned char ch) { return !std::isspace(ch); }).base(),
                value.end());

            // Trim trailing whitespace from key
            key.erase(std::find_if(key.rbegin(), key.rend(),
                [](unsigned char ch) { return !std::isspace(ch); }).base(),
                key.end());

            if (key.empty()) {
                errorFlag = true;
                state = ParseState::Error;
                return false;
            }

            request.headers[key] = value;
        }
    }

    bool Parser::parseBody()
    {
        const std::size_t availableBytes = buffer.size() - parsedBytes;

        if (availableBytes < contentLength) {
            return false;
        }

        request.body = buffer.substr(parsedBytes, contentLength);
        parsedBytes += contentLength;

        state = ParseState::Complete;
        return true;
    }

    // ================================
    // Utility Helpers
    // ================================
    std::size_t Parser::findCRLF(const std::string& str, std::size_t start) const
    {
        return str.find("\r\n", start);
    }

    std::size_t Parser::findDoubleCRLF(const std::string& str) const
    {
        return str.find("\r\n\r\n");
    }
}