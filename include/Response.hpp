#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Includes.hpp"
#include "Logger.hpp"

class Response {
    protected:
        int _clientSocket;
        Logger &_logger;

    public:
        Response(int clientSocket, Logger &logger);
        virtual ~Response();

        virtual void prepareResponse() = 0;
        virtual void sendResponse() = 0;
};

#endif // RESPONSE_HPP
