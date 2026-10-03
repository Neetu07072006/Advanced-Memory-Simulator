#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include "SimulationAPI.h"

class HttpServer
{
    SimulationAPI& api;
    int port;
    bool running;

#ifdef _WIN32
    void* serverSocket;
#else
    int serverSocket;
#endif

    std::string urlDecode(const std::string& value) const;
    std::string getParameter(const std::string& query,const std::string& key) const;
    std::string handleRequest(const std::string& method,const std::string& target);
    std::string makeResponse(int status,const std::string& body) const;
    std::string statusText(int status) const;

public:
    HttpServer(SimulationAPI& a,int p=8080);
    ~HttpServer();

    bool start();
    void stop();
};

#endif