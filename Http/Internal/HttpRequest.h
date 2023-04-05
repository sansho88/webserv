//
// Created by Alexis Lavaud on 04/04/2023.
//

#ifndef WEBSERV_HTTPREQUEST_H
#define WEBSERV_HTTPREQUEST_H
#include "../Request.h"

class HttpRequest : public Request
{
public:
    HttpRequest(HttpMethod method, const std::string &rawPath, HttpVersion version, tHttpHeaders &headers);
    ~HttpRequest();
};


#endif //WEBSERV_HTTPREQUEST_H
