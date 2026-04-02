#include "../include/Request.h"

/*=====================================================================================

    CONSTRUCTOR:
        Sets all members to safe defaults.

======================================================================================*/

Request::Request()
    : method(""),
      path(""),
      version(""),
      headers(),
      body("")
{

}

/*=====================================================================================

    RESET FUNCTION:
        Clears all members.

======================================================================================*/

void Request::reset()
{
    method.clear();
    path.clear();
    version.clear();
    headers.clear();
    body.clear();
}

/*=====================================================================================

    HAS HEADERS FUNCTION:
        Checks to see if headers are in raw data.

======================================================================================*/

bool Request::hasHeader(const std::string& key) const
{
    return headers.find(normalizeHeaderKey(key)) != headers.end();
}

/*=====================================================================================

    GET HEADERS FUNCTION:
        Returns the headers.

======================================================================================*/

std::string Request::getHeader(const std::string& key) const
{
    auto it = headers.find(normalizeHeaderKey(key));

    if (it != headers.end()) {
        return it->second;
    }

    return "";
}

/*=====================================================================================

    NORMALIZE HEADER KEY FUNCTION:
        Handles case-sensitivity.

======================================================================================*/

std::string Request::normalizeHeaderKey(const std::string& key)
{
    std::string normalized = key;

    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
        [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });

    return normalized;
}