#include "../include/Router.h"

/*=====================================================================================

    CONSTRUCTOR:
        Default constructor. No special setup required.

======================================================================================*/

Router::Router()
    : routes()
{
}

/*=====================================================================================

    ADD ROUTE FUNCTION:
        Internal helper to register a route under a given HTTP method and path.

======================================================================================*/

void Router::addRoute(const std::string& method,
    const std::string& path,
    RouteHandler handler)
{
    routes[method][path] = handler;
}

/*=====================================================================================

    GET ROUTE FUNCTION:
        Register a GET route.

======================================================================================*/

void Router::get(const std::string& path, RouteHandler handler)
{
    addRoute("GET", path, handler);
}

/*=====================================================================================

    POST ROUTE FUNCTION:
        Register a POST route.

======================================================================================*/

void Router::post(const std::string& path, RouteHandler handler)
{
    addRoute("POST", path, handler);
}

/*=====================================================================================

    PUT ROUTE FUNCTION:
        Register a PUT route.

======================================================================================*/

void Router::put(const std::string& path, RouteHandler handler)
{
    addRoute("PUT", path, handler);
}

/*=====================================================================================

    PATCH ROUTE FUNCTION:
        Register a PATCH route.

======================================================================================*/

void Router::patch(const std::string& path, RouteHandler handler)
{
    addRoute("PATCH", path, handler);
}

/*=====================================================================================

    REMOVE ROUTE FUNCTION:
        Register a DELETE route. Named 'remove' because 'delete' is a C++ keyword.

======================================================================================*/

void Router::remove(const std::string& path, RouteHandler handler)
{
    addRoute("DELETE", path, handler);
}

/*=====================================================================================

    ROUTE FUNCTION:
        Find and execute the matching handler for the incoming request.
        Returns:
            1. Matching route response if found.
            2. 405 if path exists under another method.
            3. 404 if no route exists.

======================================================================================*/

Response Router::route(const Request& request) const
{
    auto methodIt = routes.find(request.method);

    if (methodIt != routes.end()) {
        const auto& methodRoutes = methodIt->second;

        auto pathIt = methodRoutes.find(request.path);
        if (pathIt != methodRoutes.end()) {
            return pathIt->second(request);
        }
    }

    if (pathExistsInAnotherMethod(request.method, request.path)) {
        return makeMethodNotAllowedResponse();
    }

    return makeNotFoundResponse();
}

/*=====================================================================================

    PATH EXISTS IN ANOTHER METHOD FUNCTION:
        Check whether a path exists, but under a different HTTP method.

======================================================================================*/

bool Router::pathExistsInAnotherMethod(const std::string& method,
    const std::string& path) const
{
    for (const auto& methodEntry : routes) {
        if (methodEntry.first == method) {
            continue;
        }

        const auto& methodRoutes = methodEntry.second;

        if (methodRoutes.find(path) != methodRoutes.end()) {
            return true;
        }
    }

    return false;
}

/*=====================================================================================

    NOT FOUND RESPONSE FUNCTION:
        Build a simple 404 response.

======================================================================================*/

Response Router::makeNotFoundResponse() const
{
    Response response;
    response.setStatus(404, "Not Found");
    response.setHeader("Content-Type", "text/html");
    response.body =
        "<html>"
        "<head><title>404 Not Found</title></head>"
        "<body><h1>404 Not Found</h1></body>"
        "</html>";
    return response;
}

/*=====================================================================================

    METHOD NOT ALLOWED RESPONSE FUNCTION:
        Build a simple 405 response.

======================================================================================*/

Response Router::makeMethodNotAllowedResponse() const
{
    Response response;
    response.setStatus(405, "Method Not Allowed");
    response.setHeader("Content-Type", "text/html");
    response.body =
        "<html>"
        "<head><title>405 Method Not Allowed</title></head>"
        "<body><h1>405 Method Not Allowed</h1></body>"
        "</html>";
    return response;
}