#include "ServerManager.hpp"
#include "Globals.hpp"
#include "Includes.hpp"

ServerManager::ServerManager(const std::string &configFilePath)
	: _logger(new Logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE)),
	  _epollManager(*_logger),			// Mudei a ordem aqui
	  _config(configFilePath, *_logger) // E aqui
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
	_clientServerMap[clientSocket] = serverSocket;
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
		// std::cout << "Event received on fd: " << events[i].data.fd << std::endl;
		if (_fds.isFdInServer(events[i].data.fd))
		{
			if (events[i].events & EPOLLIN)
			{
				// std::cout << "EPOLLIN event on server socket: " << events[i].data.fd << std::endl;
				acceptConnection(events[i].data.fd);
			}
		}
		else
		{
			if (events[i].events & EPOLLIN)
			{
				// std::cout << "EPOLLIN event on client socket: " << events[i].data.fd << std::endl;
				handleRead(events[i].data.fd);
			}
			if (events[i].events & EPOLLOUT)
			{
				// std::cout << "EPOLLOUT event on client socket: " << events[i].data.fd << std::endl;
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
	std::cout << "read: success" << std::endl;
}

void ServerManager::handleError(int clientSocket, Logger *logger, const std::string &errorPage, const std::string &status)
{
	std::string response = "HTTP/1.1 " + status + " " + errorPage + "\r\n";
	std::stringstream ss;
	std::string body = readFile(errorPage);
	ss << body.size();
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += body;
	int bytesWritten = send(clientSocket, response.c_str(), response.size(), 0);
	if (bytesWritten <= 0)
	{
		_epollManager.modifyEpoll(clientSocket, EPOLLIN);
		logger->logError(LOG_ERROR, "Error sending error response", true);
		// _fds.removeFdFromServer(clientSocket);
		return;
	}
	_epollManager.modifyEpoll(clientSocket, EPOLLIN);
}
void ServerManager::handleResponse(Request &request, ServerConfigs &server, int clientSocket)
{
	std::string status = request.validateRequest(_config, server);
	std::cout << "debbug 03 [" << status << "]" << std::endl;
	if (status != "")
	{
		handleError(clientSocket, _logger, server.errorPages[status], status);
		return;
	}
	if (request.isCGI())
	{
		// CGIResponse cgiResponse(clientSocket, *_logger, request, server);
		// cgiResponse.prepareResponse();
		// cgiResponse.sendResponse();
		send(clientSocket, "CGI not implemented", 19, 0);
		_epollManager.modifyEpoll(clientSocket, EPOLLIN);
	}

	switch (request.getMethod())
	{
	case GET:
	{
		std::cout << "entrou no get" << std::endl;
		GetResponse getResponse(clientSocket, _logger, request.getUri());
		getResponse.prepareResponse(request.getLocation(), server);
		getResponse.sendResponse();
		_epollManager.modifyEpoll(clientSocket, EPOLLIN);
		break;
	}
	case POST:
	{
		// PostResponse postResponse(clientSocket, *_logger, request.getUri());
		// postResponse.prepareResponse();
		// postResponse.sendResponse();
		break;
	}
	case DELETE:
	{
		// DeleteResponse deleteResponse(clientSocket, *_logger, request.getUri());
		// deleteResponse.prepareResponse();
		// deleteResponse.sendResponse();
		break;
	}
	default:
		break;
	}
}

void ServerManager::handleWrite(int clientSocket)
{
	Request request(_requestMap[clientSocket]);
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		if ((*it)->getServerSocket() == _clientServerMap[clientSocket])
		{
			handleResponse(request, (*it)->getConfig(), clientSocket);
			_requestMap[clientSocket] = "";
			break;
		}
	}

	// int bytesWritten = send(clientSocket, response.c_str(), response.size(), 0);
	// if (bytesWritten <= 0)
	// {
	// 	_epollManager.modifyEpoll(clientSocket, EPOLLIN);
	// 	// _fds.removeFdFromServer(clientSocket);
	// 	return;
	// }
}