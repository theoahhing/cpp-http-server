#include "../include/Response.h"

/*=====================================================================================

    CONSTRUCTOR:
        Sets all members to safe defaults.

======================================================================================*/

Response::Response()
    : version("HTTP/1.1"),
      statusCode(200),
      reasonPhrase("OK"),
      headers(),
      body("")
{
}

/*=====================================================================================

    RESET FUNCTION:
        Clears all members.

======================================================================================*/

void Response::reset()
{
    version = "HTTP/1.1";
    statusCode = 200;
    reasonPhrase = "OK";
    headers.clear();
    body.clear();
}

/*=====================================================================================

    SET STATUS FUNCTION:
        Sets the responses status code and phrase.

======================================================================================*/

void Response::setStatus(int code, const std::string& phrase)
{
    statusCode = code;
    reasonPhrase = phrase;
}

/*=====================================================================================

    SET HEADER FUNCTION:
        Add header to the list.

======================================================================================*/

void Response::setHeader(const std::string& key, const std::string& value)
{
    headers[normalizeHeaderKey(key)] = value;
}

/*=====================================================================================

    HAS HEADER FUNCTION:
        Checks to see if headers are in raw data.

======================================================================================*/

bool Response::hasHeader(const std::string& key) const
{
    return headers.find(normalizeHeaderKey(key)) != headers.end();
}

/*=====================================================================================

    GET HEADER FUNCTION:
        Returns the headers.

======================================================================================*/

std::string Response::getHeader(const std::string& key) const
{
    auto it = headers.find(normalizeHeaderKey(key));

    if (it != headers.end()) {
        return it->second;
    }

    return "";
}


/*=====================================================================================

    TO STRING FUNCTION:
        Converts all the data for response to string.

======================================================================================*/

std::string Response::toString() const
{
    std::ostringstream responseStream;

    responseStream << version << " "
        << statusCode << " "
        << reasonPhrase << "\r\n";

    std::map<std::string, std::string> finalHeaders = headers;

    finalHeaders["content-length"] = std::to_string(body.size());

    if (finalHeaders.find("connection") == finalHeaders.end()) {
        finalHeaders["connection"] = "close";
    }

    for (const auto& header : finalHeaders) {
        responseStream << header.first << ": "
            << header.second << "\r\n";
    }

    responseStream << "\r\n";
    responseStream << body;

    return responseStream.str();
}

/*=====================================================================================

    NORMALIZE HEADER KEY FUNCTION:
        Handles case-sensitivity.

======================================================================================*/

std::string Response::normalizeHeaderKey(const std::string& key)
{
    std::string normalized = key;

    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
        [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });

    return normalized;
}