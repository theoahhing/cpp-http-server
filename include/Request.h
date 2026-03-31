#pragma once

/*
    The request file is to identify what the client is wanting to do. The common HTTP methods are:
        - GET: Retrieve something
        - POST: Submit data or create something
        - PUT: Replace or update something
        - PATCH: Partially update something
        - DELETE: Remove something
        - HEAD: Get headers only
        - OPTIONS: Asks what methods are allowed
        - TRACE: Diagnostic, usually ignored or disabled
        - CONNECT: Mainly for proxies or tunnels

    
*/

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