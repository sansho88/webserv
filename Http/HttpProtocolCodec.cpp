//
// Created by Alexis Lavaud on 23/03/2023.
//

#include "HttpProtocolCodec.h"
#include <sstream>
#include "HttpClientHandler.h"
#include "../IO/DataBuffer.h"
#include "Request.h"
#include "Response.h"
#include "../IO/BufferEvent.h"
#include "AsyncRequestHandler.h"
#include "../string_utils.hpp"
#include "HttpException.h"
#include <stdio.h>

HttpProtocolCodec::HttpProtocolCodec(HttpClientHandler *handler, DataBuffer *input, DataBuffer *output)
        : m_handler(handler), m_inputBuffer(input), m_outputBuffer(output), m_bufferEvent(handler->GetBufferEvent())
{
    m_requestHeaderParsed = false;
    m_asyncHandler = NULL;
    m_headersSent = false;
    m_pendingFinalizeResponse = false;

    m_methods.insert(std::make_pair("GET", kHttpMethod_Get));
    m_methods.insert(std::make_pair("POST", kHttpMethod_Post));
    m_methods.insert(std::make_pair("PUT", kHttpMethod_Put));
    m_methods.insert(std::make_pair("PATCH", kHttpMethod_Patch));
    m_methods.insert(std::make_pair("DELETE", kHttpMethod_Delete));
    m_methods.insert(std::make_pair("HEAD", kHttpMethod_Head));
    m_methods.insert(std::make_pair("OPTIONS", kHttpMethod_Options));
}

HttpProtocolCodec::~HttpProtocolCodec()
{
    delete m_asyncHandler;
}

void HttpProtocolCodec::ProcessDataInput()
{
    std::string line;
    bool dispatchRequest;

    dispatchRequest = false;
    while (m_inputBuffer->Readln(line))
    {
        if (!m_requestHeaderParsed)
        {
            ParseRequestHeader(line);
            m_requestHeaderParsed = true;

            if (HttpVersion::kHttpVersion_1_0 == m_httpVersion)
            {
                dispatchRequest = true;
            }
        }
        else
        {
            if (line.empty())
                dispatchRequest = true;
            else
                ParseHeader(line);
        }
        if (dispatchRequest)
        {
            dispatchRequest = false;
            DispatchRequest();
            if (m_asyncHandler)
                return ;
        }
    }
    if (m_inputBuffer->GetLength() >= m_inputBuffer->GetReadHighWatermark())
    {
        /* TODO handle veryyyy looong requests */
       // printf("Buffer full and no line could be read\n");
        //m_event->GetOutputBuffer()->PutString("Buffer full");
       // Disconnect(true);
    }
}

void HttpProtocolCodec::OnOutputDrained()
{
    if (m_asyncHandler)
    {
        m_asyncHandler->OnOutputDrained();
    }
    if (m_pendingFinalizeResponse)
        FinalizeResponse();
}

void HttpProtocolCodec::ParseRequestHeader(const std::string &line)
{
    size_t methodSep;
    size_t httpVersionSep;
    std::string method;
    std::string httpVersion;
    std::string rawPath;

    methodSep = line.find_first_of(' ');
    httpVersionSep = line.find_last_of(' ');
    httpVersion = "HTTP/1.0";
    if (methodSep == std::string::npos)
    {
        printf("Invalid request !!\n");
        /* TODO invalid request */
        return ;
    }
    method = line.substr(0, methodSep);
    if (httpVersionSep != std::string::npos && httpVersionSep != methodSep)
    {
        httpVersion = line.substr(httpVersionSep + 1);
        rawPath = line.substr(methodSep, line.size() - methodSep - httpVersion.size());
    }
    else
    {
        rawPath = line.substr(methodSep + 1);
    }
    SetRequestHeader(method, rawPath, httpVersion);
}

void HttpProtocolCodec::SetRequestHeader(const std::string &method, const std::string &rawPath,
                                         const std::string &httpVersion)
{
    if (!m_httpVersion.Parse(httpVersion))
    {
        /* TODO printf("Invalid request : Unknown http version\n"); */
        return ;
    }

    tHttpMethodsMap::const_iterator methodsIt = m_methods.find(method);
    if (methodsIt == m_methods.end())
    {
        /* TODO printf("Invalid request : Unknown http method\n"); */
        return ;
    }

    m_method = methodsIt->second;
    m_rawPath = rawPath;
    utils::trim(m_rawPath);
}

void HttpProtocolCodec::ParseHeader(const std::string &line)
{
    size_t sep;
    std::string key;
    std::string value;

    sep = line.find_first_of(':');
    if (sep != std::string::npos)
    {
        key = line.substr(0, sep);
        value = line.substr(sep + 1);
    }
    else
    {
        key = line;
    }
    utils::trim(key);
    utils::trim(value);
    m_headers.insert(std::make_pair(key, value));
}

void HttpProtocolCodec::WriteResponseHeader()
{
    std::stringstream ss;
    tHeaderMap::const_iterator it;

    if (m_headersSent)
        return ;
    m_headersSent = true;

    ss << m_httpVersion << " " << m_responseStatus << " " << m_responseMessage << "\r\n";
    for (it = m_responseHeaders.begin(); it != m_responseHeaders.end(); ++it)
    {
        ss << it->first << ": " << it->second << "\r\n";
    }
    ss << "\r\n";
    m_outputBuffer->PutString(ss.str());
}

void HttpProtocolCodec::DispatchRequest()
{
    Request request(m_handler, m_headers);
    Response response(m_handler);
    tHeaderMap headers;
    IAsyncRequestHandler *asyncHandler;
    DataBuffer *body;

    request.SetMethod(m_method);
    request.SetRawPath(m_rawPath);
    request.SetProtocolVersion(m_httpVersion);

    /* Disable reading while processing the request */
    m_bufferEvent->Enable(kEvent_Write);

    try {
        m_handler->HandleRequest(&request, &response);
    }
    catch (HttpException &e)
    {
        /* TODO unhandled exception */
        /* Exception shall be handled inside ServerHost */
        /* so we should never have to catch it here */
        printf("Exception with code : %d\n", e.GetStatus());
    }
    catch (...)
    {
        printf("Unexpected error\n");
    }

    m_responseHeaders.swap(response.GetHeaders());

    if (response.IsChunked())
    {
        m_responseHeaders["Transfer-Encoding"] = "chunked";
    }
    else
    {
        m_responseHeaders["Content-Length"] = utils::to_string(response.GetContentLength());
    }

    m_responseStatus = response.GetStatusCode();
    m_responseMessage = response.GetStatusMessage();

    m_asyncHandler = response.GetAsyncHandler();
    m_chunked = response.IsChunked();

    body = response.GetBodyBuffer();
    if (body->GetLength() > 0)
    {
        WriteResponseHeader();
        Write(body);
        FinalizeResponse();
    }
}

void HttpProtocolCodec::FinalizeResponse()
{
    tHeaderMap::const_iterator it;

    if (!m_pendingFinalizeResponse)
    {
        if (!m_headersSent)
            WriteResponseHeader();

        if (m_chunked)
        {
            WriteChunkHeader(0);
            m_outputBuffer->PutString("\r\n");
        }
    }

    if (m_outputBuffer->GetLength() > 0)
    {
        m_pendingFinalizeResponse = true;
        return ;
    }

    delete m_asyncHandler;
    m_asyncHandler = NULL;

    if (HttpVersion::kHttpVersion_1_0 == m_httpVersion)
    {
        m_handler->Disconnect(true);
        return ;
    }
    else if (HttpVersion::kHttpVersion_1_1 == m_httpVersion)
    {
        it = m_headers.find("Connection");

        if (it != m_headers.end() && it->second == "close")
        {
            m_handler->Disconnect(true);
            return ;
        }
    }

    m_headers.clear();
    m_requestHeaderParsed = false;
    m_pendingFinalizeResponse = false;
    m_headersSent = false;

    /* Re-enable reads to receive the next request */
    m_bufferEvent->Enable(kEvent_Read);
}

void HttpProtocolCodec::Write(const void *data, size_t n)
{
    if (!m_headersSent)
        WriteResponseHeader();
    m_outputBuffer->Write(data, n);
}

void HttpProtocolCodec::Write(DataBuffer *buffer)
{
    if (!m_headersSent)
        WriteResponseHeader();

    if (m_chunked)
    {
        WriteChunkHeader(buffer->GetLength());
    }
    m_outputBuffer->AddBuffer(buffer);
    if (m_chunked)
    {
        m_outputBuffer->PutString("\r\n");
    }
}

void HttpProtocolCodec::WriteChunkHeader(size_t length)
{
    std::stringstream ss;

    ss << std::hex << length << "\r\n";
    m_outputBuffer->PutString(ss.str());
}

DataBuffer *HttpProtocolCodec::GetInputBuffer() const {
    return m_inputBuffer;
}

DataBuffer *HttpProtocolCodec::GetOutputBuffer() const {
    return m_outputBuffer;
}

void HttpProtocolCodec::AddHeader(const std::string &name, const std::string &value)
{
    m_responseHeaders[name] = value;
}
