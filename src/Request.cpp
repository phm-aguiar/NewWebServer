#include "Request.hpp"

Request::Request(const std::string &rawRequest) : _rawRequest(rawRequest)
{
	_method = INVALID;
	_isCGI = false;
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

std::string Request::folderPath()
{
	if (_uri == "/")
		return _uri;
	std::string folderPath = _uri;
	if (folderPath[folderPath.size() - 1] == '/')
	{
		folderPath = folderPath.substr(0, folderPath.size() - 1);
	}
	size_t pos = folderPath.find_last_of('/');
	if (pos != std::string::npos)
	{
		folderPath = folderPath.substr(0, pos);
	}
	return folderPath;
}

std::string Request::validateRequest(Config _config, ServerConfigs server)
{
	static int counter = 0;
	std::string error = "";
	bool locationFound = false;
	std::cout << "passou aki " << counter++ << std::endl;
	std::cout << _uri << std::endl;
	std::cout << folderPath() << std::endl;
	_location = _config.getLocationConfig(server, folderPath(), locationFound);
	if (!locationFound)
	{
		error = "404";
	}
	if (std::find(_location.methods.begin(), _location.methods.end(), getMethod()) == _location.methods.end())
	{
		error = "405";
	}
	if (_location.cgiEnabled)
	{
		_isCGI = true;
	}
	return error;
}

void Request::clear()
{
	_method = INVALID;
	_uri.clear();
	_headers.clear();
	_body.clear();
	_rawRequest.clear();
}

