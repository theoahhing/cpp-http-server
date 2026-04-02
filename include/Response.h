#pragma once
/*
	The Response structure is to build a response based on the request received. After the
	request is pasred, the response will identify the action to take and the response will
	be formatted in the same form. Firstly, it will store the following information:
		- HTTP version
		- Status Code
		- Reason Phrase
		- Headers
		- Body

	HTTP response to contain:
		1. Status Line - First line of the response includes:
			1.1: HTTP version used.
			1.2: Status Code indicating success or failure.
			1.3: Reason Phrase describing the status code.
		2. HTTP Headers - Key value pairs that provide metadata about the response and the
		   server. These include:
			2.1: Content-Type specifying the media type of the resource.
			2.2: Content-Length indicating the size (bytes) of the response.
			2.3: Date.
			2.4: Server information.
		3. Message Body - The actual content or resource being returned to the client. Usually
		   includes:
			3.1: Usage containing HTML for a webpage, JSON for an API, or binary data for images.
			3.2: When omitted.

	The HTTP version is usually sent with the request. As for the status codes, a few standard
	codes to remember are:
		- Successful:
			* 200 OK
			* 201 Created
			* 204 No Content
		- Redirects:
			* 301 Moved Permanently
			* 302 Found
		- Client Errors:
			* 400 Bad Request
			* 403 Forbidden
			* 404 Not Found
			* 405 Method Not Allowed
		- Server Errors:
			* 500 Internal Server Error
			* 501 Not Implemented
	
	Handler to follow:
		1. If parse request fails -> 400
		2. If unsupported by server -> 501
		3. No route/resource -> 404
		4. If method is not allowed for that route ->
		5. If handler is:
			5.1: Successful with content -> 200
			5.2: Successful with creation -> 201
			5.3: Successful with no body -> 204
			5.4: Unexpected failure -> 500
*/

#ifndef _RESPONSE_H
#define _RESPONSE_H

#include "CommonIncludes.h"

struct Response
{
	std::string version;
	int statusCode;
	std::string reasonPhrase;
	std::map<std::string, std::string> headers;
	std::string body;

	Response();

	void reset();

	void setStatus(int code, const std::string& phrase);
	void setHeader(const std::string& key, const std::string& value);

	bool hasHeader(const std::string& key) const;
	std::string getHeader(const std::string& key) const;

	std::string toString() const;

private:
	static std::string normalizeHeaderKey(const std::string& key);
};

#endif _RESPONSE_H
