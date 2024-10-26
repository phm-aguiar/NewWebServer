#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Config.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "EpollManager.hpp"
#include "Fds.hpp"
#include "Request.hpp"
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

    Logger *_logger;
    Config _config;
    EpollManager _epollManager;
    Fds _fds;
    std::vector<Server*> _servers;
	std::map<int, std::string> _requestMap;
};

#endif // SERVERMANAGER_HPP