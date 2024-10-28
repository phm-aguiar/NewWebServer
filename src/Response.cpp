#include "Response.hpp"

Response::Response(int clientSocket, Logger &logger) : _clientSocket(clientSocket), _logger(logger), _status_code("200"), _reason_phrase("OK") {}

Response::~Response() {}

std::string Response::generateResponse() const
{
	std::ostringstream response_stream;

	response_stream << "HTTP/1.1 " << _status_code << " " << _reason_phrase << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		response_stream << it->first << ": " << it->second << "\r\n";
	}
	response_stream << "\r\n";
	response_stream << _body;

	return response_stream.str();
}

void Response::handleError(std::string _status_code, const std::string &error_page, const std::string &error_message, Logger &logger)
{
	_status_code = _status_code;
	_reason_phrase = error_message;
	_body = readFile(error_page);
	std::stringstream ss;
	ss << _body.size();
	_headers["Content-Length"] = ss.str();
	_headers["Content-Type"] = "text/html";
	logger.logError(LOG_ERROR, "Error: " + error_message);
}
void Response::handleFileResponse(const std::string &path, Logger &logger)
{
	_body = readFile(path);
	_status_code = "200";
	_reason_phrase = "OK";
	std::stringstream ss;
	ss << _body.size();
	_headers["Content-Length"] = ss.str();
	_headers["Content-Type"] = getContentType(path);
	logger.logDebug(LOG_INFO, "Response body: " + _body);
}