#include "Server.hpp"
#include "Logger.hpp"
#include "Includes.hpp"
#include "Defines.hpp"
#include "Request.hpp"
#include "Config.hpp"
#include "ServerManager.hpp"
#include "Response.hpp"
#include "PostResponse.hpp"
#include "Globals.hpp"

#include "GetResponse.hpp"

volatile sig_atomic_t stop = 0;

void testPrepareResponse()
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	int clientSocket = 0; // Mock socket
	std::string filePath = "./test";

	GetResponse getResponse(clientSocket, logger, filePath);

	// Call the method to be tested
	std::string response = getResponse.prepareResponse();

	// Add assertions to verify the expected behavior
	// For example:
	if (response == "Expected response string")
	{
		std::cout << "testPrepareResponse passed" << std::endl;
	}
	else
	{
		std::cout << "testPrepareResponse failed" << std::endl;
	}
}

int main()
{
	testPrepareResponse();
	return 0;
}