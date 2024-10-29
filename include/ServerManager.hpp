#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Config.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "EpollManager.hpp"
#include "Fds.hpp"
#include "Request.hpp"
#include "GetResponse.hpp"
class ServerManager
{
public:
	ServerManager(const std::string &configFilePath);
	~ServerManager();
	void run();

private:
	bool initializeServers();
	void handleEvents();
	void handleRead(int clientSocket);
	void handleWrite(int clientSocket);
	void acceptConnection(int serverSocket);
	void handleResponse(Request &request, ServerConfigs &server, int clientSocket);
	void handleError(int clientSocket, Logger *logger, const std::string &errorPage, const std::string &status);
	void closeConnection(int clientSocket);

	Logger *_logger;
	EpollManager _epollManager;
	Config _config;
	Fds _fds;
	std::vector<Server*> _servers;
	std::map<int, std::string> _requestMap;
	std::map<int, std::string> _responseMap;
	std::map<int, bool> _connectionMap; 
	std::map<int, int> _clientServerMap;
	// quero dizer ao request que esse arquivo tem boundary
	std::map<int, bool> _boundaryMap;
};

#endif // SERVERMANAGER_HPP