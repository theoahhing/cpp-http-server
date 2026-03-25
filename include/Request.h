#pragma once


#ifndef _REQUEST_H
#define _REQUEST_H

#include "CommonIncludes.h"

struct Request
{
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

    Request();

    void reset();
    bool hasHeader(const std::string& key) const;
    std::string getHeader(const std::string& key) const;
};



#endif // _REQUEST_H