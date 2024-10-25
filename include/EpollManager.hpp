
#ifndef EPOLLMANAGER_HPP_
#define EPOLLMANAGER_HPP_

// Class declaration
#include "Logger.hpp"

// Classe encapsulando a lógica de Epoll
class EpollManager
{
	public:
		EpollManager(Logger &logger);
		~EpollManager();
		bool addToEpoll(int sockfd, uint32_t events);
		bool removeFromEpoll(int sockfd);
		void modifyEpoll(int sockfd, uint32_t events);
		int getEpollFD() const;

	private:
		int _epoll_fd;
		Logger &_logger;
		void initialize();

};

#endif // EPOLLMANAGER_HPP_
