#include "Request.hpp"

Request::Request(const std::string &rawRequest) : _rawRequest(rawRequest)
{
	_method = INVALID;
	parseRequest();
}

httpMethod Request::getMethod() const { return _method; }

const std::string &Request::getUri() const { return _uri; }

const std::string &Request::getHeader(const std::string &name) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(name);
	if (it == _headers.end())
	{
		static const std::string empty;
		return empty;
	}
	return it->second;
}

const std::map<std::string, std::string> &Request::getHeaders() const { return _headers; }

const std::string &Request::getBody() const { return _body; }

void Request::parseRequest()
{
	std::istringstream requestStream(_rawRequest);
	std::string line;
	std::vector<std::string> headerLines;
	bool body = false;

	while (std::getline(requestStream, line))
	{
		if (line.empty())
		{
			body = true;
			continue;
		}
		if (!body)
		{
			if (_method == INVALID)
			{
				parseMethodAndUri(line);
			}
			else
			{
				headerLines.push_back(line);
			}
		}
		else
		{
			_body += line + "\n";
		}
	}
	parseHeaders(headerLines);
}

void Request::parseMethodAndUri(const std::string &line)
{
	std::istringstream lineStream(line);
	std::string method;
	std::string uri;

	lineStream >> method >> uri;

	_method = parseMethod(method);
	_uri = uri;
}

void Request::parseHeaders(const std::vector<std::string> &headerLines)
{
	for (std::vector<std::string>::const_iterator it = headerLines.begin(); it != headerLines.end(); ++it)
	{
		size_t colonPos = it->find(':');
		if (colonPos != std::string::npos)
		{
			std::string name = it->substr(0, colonPos);
			std::string value = it->substr(colonPos + 2);
			_headers[name] = value;
		}
	}
}

void Request::parseBody(const std::string &body)
{
	_body = body;
}

httpMethod Request::parseMethod(const std::string &method)
{
	if (method == "GET")
	{
		return GET;
	}
	else if (method == "POST")
	{
		return POST;
	}
	else if (method == "DELETE")
	{
		return DELETE;
	}
	return INVALID;
}

