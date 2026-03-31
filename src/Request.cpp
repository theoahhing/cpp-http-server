#include "../include/Request.h"

Request::Request()
    : method(""),
    path(""),
    version(""),
    headers(),
    body("")
{

}

void Request::reset()
{
    method.clear();
    path.clear();
    version.clear();
    headers.clear();
    body.clear();
}

bool Request::hasHeader(const std::string& key) const
{
    return headers.find(key) != headers.end();
}

std::string Request::getHeader(const std::string& key) const
{
    auto it = headers.find(key);

    if (it != headers.end()) {
        return it->second;
    }

    return "";
}