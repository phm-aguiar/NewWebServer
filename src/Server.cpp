#include "Server.hpp"
#include "Globals.hpp"

std::string uuidGenerator()
{
	std::stringstream ss;
	ss << std::hex << std::uppercase;
	for (int i = 0; i < 8; i++)
	{
		ss << rand() % 16;
	}
	return ss.str();
}

Server::Server(const ServerConfigs &config, Logger &logger) : _logger(logger), _config(config)
{
	_backlog = SOMAXCONN;
	_serverSocket = -1;
	_epoll = EpollManager();
	_uuid = uuidGenerator();
}
Server::~Server()
{
	if (_serverSocket != -1)
		close(_serverSocket);
}

bool Server::initialize() { 
	if (!createSocket()) return false;
	if (!configureSocket()) return false;
	if (!bindSocket()) return false;
	if (!listenSocket()) return false;
	if(!_epoll.addToEpoll(_serverSocket, EPOLLIN)) return false;
	return true;
 }

void Server::run()
{
	std::stringstream ss;
	ss << "Server " << _uuid << COLORIZE(GREEN, " Running ");
	ss << _config.serverName << ":" << _config.port;
	_logger.logDebug(LOG_INFO, ss.str(), true);

	while (!stop)
	{
		sleep(1);
	}
}

bool Server::createSocket()
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (_serverSocket < 0)
	{
		_logger.logError(LOG_ERROR, "Error opening socket");
		return false;
	}
	_logger.logDebug(LOG_DEBUG, "Socket created");
	return true;
}

bool Server::logErrorAndClose(const std::string &message)
{

	close(_serverSocket);
	_logger.logError(LOG_ERROR, message);
	return false;
}

bool Server::bindSocket()
{
	logStream log;
	_serv_addr.sin_family = AF_INET;
	_serv_addr.sin_addr.s_addr = INADDR_ANY;
	_serv_addr.sin_port = htons(_config.port);
	if (bind(_serverSocket, (sockAddr *)&_serv_addr, sizeof(_serv_addr)) < 0)
	{
		log << "Error on binding to port " << _config.port;
		return logErrorAndClose(log.str());
	}
	log << "Binded to port " << _config.port;
	_logger.logDebug(LOG_DEBUG, log.str(), true);
	return true;
}

bool Server::listenSocket()
{
	logStream log;
	if (listen(_serverSocket, _backlog) < 0)
	{
		log << "Error on listening to port " << _config.port;
		return logErrorAndClose(log.str());
	}
	log << "Listening on port " << _config.port;
	_logger.logDebug(LOG_DEBUG, log.str(), true);
	return true;
}


bool Server::configureSocket()
{
	int opt = 1;
	logStream log;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		log << "Error on setting socket options";
		return logErrorAndClose(log.str());
	}
	log << "Socket options set";
	_logger.logDebug(LOG_DEBUG, log.str(), true);
	return true;
}

std::string Server::getUuid()
{
	return _uuid;
}


void Server::acceptConnection()
{
	sockaddrIn client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_socket = accept(_serverSocket, (sockAddr *)&client_addr, &client_len);
	if (client_socket < 0)
	{
		_logger.logError(LOG_ERROR, "Error accepting connection");
		return;
	}
	_logger.logDebug(LOG_DEBUG, "Accepted connection", true);
	_epoll.addToEpoll(client_socket, EPOLLIN);
}

