#ifndef GETRESPONSE_HPP
#define GETRESPONSE_HPP

#include "Response.hpp"
#include <string>

class GetResponse : public Response {
    private:
        std::string _filePath;
        std::string _responseData;

    public:
        GetResponse(int clientSocket, Logger &logger, const std::string &filePath);

        void prepareResponse();
        void sendResponse();
};

#endif // GETRESPONSE_HPP
