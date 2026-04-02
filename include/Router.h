#pragma once
/*
	Route file to do the following:
		1. Receive a parse request.
		2. Decide which handler matches.
		3. Return a response.

	Set up a handler type so each route handler:
		- Takes a request
		- Returns a response
*/

#ifndef _ROUTER_H
#define _ROUTER_H

#include "Request.h"
#include "Response.h"

class Router
{
public:
    using RouteHandler = std::function<Response(const Request&)>;

    Router();
    ~Router() = default;

    void get(const std::string& path, RouteHandler handler);
    void post(const std::string& path, RouteHandler handler);
    void put(const std::string& path, RouteHandler handler);
    void patch(const std::string& path, RouteHandler handler);
    void remove(const std::string& path, RouteHandler handler);

    Response route(const Request& request) const;

private:
    void addRoute(const std::string& method,
        const std::string& path,
        RouteHandler handler);

    bool pathExistsInAnotherMethod(const std::string& method,
        const std::string& path) const;

    Response makeNotFoundResponse() const;
    Response makeMethodNotAllowedResponse() const;

private:
    std::map<std::string, std::map<std::string, RouteHandler>> routes;
};

#endif