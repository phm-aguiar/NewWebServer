#include "ServerManager.hpp"
#include "Globals.hpp"
#include "Includes.hpp"

ServerManager::ServerManager(const std::string &configFilePath)
	: _logger(new Logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE)),
	  _config(configFilePath, *_logger),
	  _epollManager(*_logger)
{
	if (!initializeServers())
	{
		throw std::runtime_error("Failed to initialize servers");
	}
}

ServerManager::~ServerManager()
{
	delete _logger;
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		delete *it;
	}
}

bool ServerManager::initializeServers()
{
	std::vector<ServerConfigs> serverConfigs = _config.getServers();

	for (std::vector<ServerConfigs>::iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
	{
		// Alocar dinamicamente as instâncias de Server e armazená-las no vetor _servers
		Server *server = new Server(*it, *_logger, _epollManager);
		if (!server->initialize())
		{
			delete server;
			return false;
		}
		std::cout << server->getServerSocket() << std::endl;
		_servers.push_back(server);
		_fds.addFdToServer(server->getServerSocket());
		std::cout << "Server initialized and added to epoll: " << server->getServerSocket() << std::endl;
	}

	return true;
}

void ServerManager::run()
{
	while (!stop)
	{
		handleEvents();
	}
}

void ServerManager::acceptConnection(int serverSocket)
{
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);
	if (clientSocket == -1)
	{
		_logger->logError(LOG_ERROR, "Error accepting connection", true);
		return;
	}
	_epollManager.addToEpoll(clientSocket, EPOLLIN);
	_requestMap[clientSocket] = "";
	// _fds.addFdToServer(clientSocket);
}

void ServerManager::handleEvents()
{
	epoll_event events[MAX_EVENTS];
	int nfds = epoll_wait(_epollManager.getEpollFD(), events, MAX_EVENTS, -1);
	if (nfds == -1)
	{
		_logger->logError(LOG_ERROR, "Error on epoll_wait", true);
		return;
	}
	for (int i = 0; i < nfds; i++)
	{
		std::cout << "Event received on fd: " << events[i].data.fd << std::endl;
		if (_fds.isFdInServer(events[i].data.fd))
		{
			if (events[i].events & EPOLLIN)
			{
				std::cout << "EPOLLIN event on server socket: " << events[i].data.fd << std::endl;
				acceptConnection(events[i].data.fd);
			}
		}
		else
		{
			if (events[i].events & EPOLLIN)
			{
				std::cout << "EPOLLIN event on client socket: " << events[i].data.fd << std::endl;
				handleRead(events[i].data.fd);
			}
			if (events[i].events & EPOLLOUT)
			{
				std::cout << "EPOLLOUT event on client socket: " << events[i].data.fd << std::endl;
				handleWrite(events[i].data.fd);
			}
		}
	}
}

void ServerManager::handleRead(int clientSocket)
{
	std::cout << "Handling read on socket: " << clientSocket << std::endl;
	char buffer[65535];
	bzero(buffer, sizeof(buffer));
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

	if (bytesRead == -1)
	{
		_epollManager.removeFromEpoll(clientSocket);
		close(clientSocket);
		return;
	}
	else if (bytesRead == 0)
	{
		_epollManager.removeFromEpoll(clientSocket);
		close(clientSocket);
		return;
	}
	else
	{
		std::string lenfVerif = std::string(buffer, bytesRead);
		if (lenfVerif.size() < 65535)
		{
			lenfVerif[lenfVerif.size() - 1] = '\0';
			_epollManager.modifyEpoll(clientSocket, EPOLLOUT);
			_requestMap[clientSocket] += lenfVerif;

		}
	}

	std::cout << "debbug 02" << std::endl;
	_requestMap[clientSocket] += std::string(buffer, bytesRead);
	// std::cout << "Received data: " << std::string(buffer, bytesRead) << std::endl;
}

void ServerManager::handleWrite(int clientSocket)
{
	Request request(_requestMap[clientSocket]);
	std::cout << "Handling write on socket: " << clientSocket << std::endl;
	std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 38\r\n\r\n";

	std::string body = "<html><body><h1>Hello, World!</h1></body></html>";
	std::string response = header + body;

	int bytesWritten = send(clientSocket, response.c_str(), response.size(), 0);
	if (bytesWritten <= 0)
	{
		_epollManager.modifyEpoll(clientSocket, EPOLLIN);
		// _fds.removeFdFromServer(clientSocket);
		return;
	}
}