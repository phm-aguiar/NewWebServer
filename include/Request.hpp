#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Includes.hpp"
#include "Config.hpp"

class Request
{

private:
	httpMethod _method;								 // Método HTTP (GET, POST, DELETE)
	std::string _uri;							 // URI da requisição
	std::map<std::string, std::string> _headers; // Cabeçalhos HTTP
	std::string _body;							 // Corpo da requisição, se houver
	std::string _rawRequest;					 // Requisição bruta para parseamento

public:
	// Construtor que recebe a requisição bruta
	Request(const std::string &rawRequest);

	// Métodos para acessar os dados da requisição
	httpMethod getMethod() const;									  // Retorna o método da requisição
	const std::string &getUri() const;							  // Retorna a URI
	const std::string &getHeader(const std::string &name) const;  // Retorna o valor de um cabeçalho específico
	const std::map<std::string, std::string> &getHeaders() const; // Retorna todos os cabeçalhos
	const std::string &getBody() const;							  // Retorna o corpo da requisição

private:
	// Método privado para fazer o parsing da requisição
	void parseRequest();
	void parseMethodAndUri(const std::string &line);				// Extrai o método e a URI da linha inicial
	void parseHeaders(const std::vector<std::string> &headerLines); // Extrai os cabeçalhos
	void parseBody(const std::string &body);						// Extrai o corpo da requisição, se houver

	// Funções auxiliares
	httpMethod parseMethod(const std::string &method); // Converte string para enum Method
};

#endif // REQUEST_HPP
