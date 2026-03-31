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

#include "Request.h"

struct Response
{
	
};




#endif _RESPONSE_H
