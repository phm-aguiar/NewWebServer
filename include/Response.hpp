#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "Includes.hpp"
#include "Request.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "HttpMessage.hpp"

class Response : public HttpMessage
{
public:
	// Construtor
	Response();

	// Métodos para configurar a resposta
	void setStatus(int code, const std::string &reason);
	std::string getReasonPhrase() const;
	int getStatusCode() const;

	// Geração da resposta completa
	std::string generateResponse() const;

	// Tratamento da resposta com base na requisição
	void processRequest(Request &request, const ServerConfigs *respconfig, Logger &logger);

	// Métodos auxiliares para erros
	void handleError(int status_code, const std::string &error_page, const std::string &error_message, Logger &logger);
	void handleFileResponse(const std::string &path, Logger &logger);

private:
	int _status_code;
	std::string _reason_phrase;
	std::string _root;

	// Métodos internos auxiliares
	void setBodyWithContentType(const std::string &bodyContent, const std::string &path);
	void handlerValidRequest(Request &request, Logger &logger);
	bool validMethod(const LocationConfigs it, const std::string &method);
	LocationConfigs returnLocationConfig(const ServerConfigs *respconfig, const std::string &uri);
	void getHandler(std::string path,const LocationConfigs &location, const ServerConfigs *respconfig, Logger &logger);
	void postHandler(const LocationConfigs &location, const Request &request, const ServerConfigs *respconfig, Logger &logger);
	void deleteHandler(std::string path, const LocationConfigs &location, const ServerConfigs *respconfig, Logger &logger);
};

#endif // RESPONSE_HPP_
