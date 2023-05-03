//
// Created by Alexis Lavaud on 04/04/2023.
//

#ifndef WEBSERV_HTTPCLIENT_H
#define WEBSERV_HTTPCLIENT_H
#include <string>
#include <stdint.h>

class IEventLoop;
class BufferEvent;

class HttpClient
{
public:
    HttpClient(IEventLoop *eventLoop, const char *host, uint16_t port);
    ~HttpClient();

    void DoRequest(const char *method, const char *path);

private:
    IEventLoop *m_eventLoop;
    BufferEvent *m_event;
    std::string m_host;
    uint16_t m_port;
};


#endif //WEBSERV_HTTPCLIENT_H
