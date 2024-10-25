#ifndef SERVER_HPP
#define SERVER_HPP
#include "Includes.hpp"
#include "Utils.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "EpollManager.hpp"
#include "Request.hpp"
#include <vector>
#include <sstream>
class Logger;

class Server
{
	public:
		Server(const ServerConfigs &config, Logger &logger);
		~Server();
		bool initialize();
		void run();
		std::string getUuid();

	private:
		// Métodos de manipulação de sockets e conexões
		bool createSocket();
		bool configureSocket();
		bool bindSocket();
		bool listenSocket();
		void acceptConnection(); // Aceita novas conexões

		// Métodos de processamento de eventos (Epoll)
		void handleEvents();	 // Lida com eventos epoll
		void handleRead(int clientSocket);   // Processa dados recebidos
		void handleWrite(int clientSocket);  // Envia dados ao cliente
		void closeConnection(int clientSocket);  // Fecha uma conexão

		// Métodos auxiliares
		bool logErrorAndClose(const std::string &message);
		std::string inetNtop(uint32_t binary_ip);  // Converte IP binário para string
		bool inetPton(const std::string &ip_str);  // Valida IP

		// Atributos privados
		int _serverSocket;	   // Socket principal do servidor
		int _backlog;			// Tamanho da fila de conexões
		sockaddr_in _serv_addr;   // Estrutura de endereço do servidor
		EpollManager _epoll;	 // Instância para gerenciar o epoll
		Logger &_logger;		 // Logger para debug e logs de erro
		ServerConfigs _config;   // Configurações específicas do servidor
		std::string _uuid;
};

#endif // SERVER_HPP_