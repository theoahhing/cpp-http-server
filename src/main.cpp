/*
	Go to Project -> Properties and ensure:
		Configuration: Active(Debug)											----> Change dependent on Conifg Type
		Platform: Active(x64)													----> Change dependent on Platform Type
	
	Go to Project -> Properties -> Config Properties -> General and ensure:
		Output Directory: $(ProjectDir)x64\$(Configuration)\					----> Change dependent on dir for saving final build artifacts
		Int Directory: $(ProjectDir)build\obj\$(Platform)\$(Configuration)\		----> Change dependent on dir for saving temp files during build process

	Go to Project -> Properties -> Config Properties -> Debugging and ensure:
		Working Directory: $(ProjectDir)										----> Change dependent on main working directory


	Project Description (Phase 1):
		Listen for incoming connections, receive raw request data, parse it,
		match it to predefined routes, and send back a formatted HTTP response.
	
	Runtime Pipeline:
		1. Socket accepts client
		2. Connection reads raw data
		3. HttpParser converts raw text into Request
		4. Router picks handler using Request
		5. Handler creates Response
		6. Response serializes to HTTP
		7. Connection sends it back.

	Design Philosophy:
		1. Server class
		2. Connection class
		3. HttpParser namespace
		4. Request struct
		5. Response struct (maybe class)
		6. Router class

	main.cpp to:
		1. create server object
		2. configure port/address
		3. start server
*/

#include "../include/Server.h"
#include "../include/CommonIncludes.h"

int main()
{
	Server server(8080, "127.0.0.1", 10);
	server.start();

	

	return 0;
}