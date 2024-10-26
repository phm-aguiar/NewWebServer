#include "Response.hpp"

Response::Response(int clientSocket, Logger &logger) : _clientSocket(clientSocket), _logger(logger) {}

Response::~Response() {}
