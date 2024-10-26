#ifndef POSTRESPONSE_HPP
#define POSTRESPONSE_HPP

#include "Response.hpp"
#include <string>

class PostResponse : public Response {
    private:
        std::string _postData;
        std::string _responseData;

    public:
        PostResponse(int clientSocket, Logger &logger, const std::string &postData);

        void prepareResponse();
        void sendResponse();
};

#endif // POSTRESPONSE_HPP
