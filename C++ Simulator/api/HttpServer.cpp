#include "HttpServer.h"
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <iostream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace
{
std::string boolJson(bool value)
{
    return value ? "{\"success\":true}" : "{\"success\":false}";
}

AccessType parseAccessType(const std::string& value,bool& valid)
{
    valid=true;

    if(value=="1"||value=="read"||value=="READ")
        return AccessType::READ;

    if(value=="2"||value=="write"||value=="WRITE")
        return AccessType::WRITE;

    if(value=="3"||value=="execute"||value=="EXECUTE")
        return AccessType::EXECUTE;

    valid=false;
    return AccessType::READ;
}
}

HttpServer::HttpServer(SimulationAPI& a,int p):api(a),port(p),running(false)
#ifdef _WIN32
,serverSocket(nullptr)
#else
,serverSocket(-1)
#endif
{
}

HttpServer::~HttpServer()
{
    stop();
}

std::string HttpServer::urlDecode(const std::string& value) const
{
    std::string result;

    for(size_t i=0;i<value.size();++i)
    {
        if(value[i]=='%'&&i+2<value.size())
        {
            std::string hex=value.substr(i+1,2);
            char* end=nullptr;
            long v=std::strtol(hex.c_str(),&end,16);

            if(end!=hex.c_str()&&*end=='\0')
            {
                result+=static_cast<char>(v);
                i+=2;
                continue;
            }
        }

        if(value[i]=='+')
            result+=' ';
        else
            result+=value[i];
    }

    return result;
}

std::string HttpServer::getParameter(const std::string& query,const std::string& key) const
{
    std::string search=key+"=";
    size_t start=0;

    while(start<query.size())
    {
        size_t end=query.find('&',start);

        if(end==std::string::npos)
            end=query.size();

        std::string item=query.substr(start,end-start);

        if(item.rfind(search,0)==0)
            return urlDecode(item.substr(search.size()));

        start=end+1;
    }

    return "";
}

std::string HttpServer::statusText(int status) const
{
    if(status==200) return "OK";
    if(status==201) return "Created";
    if(status==400) return "Bad Request";
    if(status==404) return "Not Found";
    if(status==405) return "Method Not Allowed";
    return "Internal Server Error";
}

std::string HttpServer::makeResponse(int status,const std::string& body) const
{
    std::ostringstream out;

    out<<"HTTP/1.1 "<<status<<" "<<statusText(status)<<"\r\n";
    out<<"Content-Type: application/json\r\n";
    out<<"Access-Control-Allow-Origin: *\r\n";
    out<<"Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    out<<"Access-Control-Allow-Headers: Content-Type\r\n";
    out<<"Content-Length: "<<body.size()<<"\r\n";
    out<<"Connection: close\r\n";
    out<<"\r\n";
    out<<body;

    return out.str();
}

std::string HttpServer::handleRequest(const std::string& method,const std::string& target)
{
    if(method=="OPTIONS")
        return makeResponse(200,"{\"success\":true}");

    size_t queryPos=target.find('?');
    std::string path=target.substr(0,queryPos);
    std::string query=queryPos==std::string::npos?"":target.substr(queryPos+1);

    if(path=="/api/health")
        return makeResponse(200,"{\"status\":\"online\",\"service\":\"AdvancedMemorySimulator\"}");

    if(method!="GET"&&method!="POST")
        return makeResponse(405,"{\"error\":\"method not allowed\"}");

    if(path=="/api/system")
    {
        if(method!="GET")
            return makeResponse(405,"{\"error\":\"method not allowed\"}");
        return makeResponse(200,api.getSystem());
    }

    if(path=="/api/processes")
    {
        if(method!="GET")
            return makeResponse(405,"{\"error\":\"method not allowed\"}");
        return makeResponse(200,api.getProcesses());
    }

    if(path=="/api/memory")
    {
        if(method!="GET")
            return makeResponse(405,"{\"error\":\"method not allowed\"}");
        return makeResponse(200,api.getMemory());
    }

    if(path=="/api/events")
    {
        if(method!="GET")
            return makeResponse(405,"{\"error\":\"method not allowed\"}");
        return makeResponse(200,api.getEvents());
    }

    if(path=="/api/schedule")
    {
        if(method=="GET")
            return makeResponse(200,api.getSchedule());

        std::string algorithmText=getParameter(query,"algorithm");
        std::string quantumText=getParameter(query,"quantum");

        if(algorithmText.empty())
            return makeResponse(400,"{\"error\":\"algorithm is required\"}");

        try
        {
            int algorithm=std::stoi(algorithmText);
            int quantum=quantumText.empty()?2:std::stoi(quantumText);

            api.runScheduling(algorithm,quantum);
            return makeResponse(200,api.getSchedule());
        }
        catch(...)
        {
            return makeResponse(400,"{\"error\":\"invalid scheduling parameters\"}");
        }
    }

    if(path=="/api/global-frames")
    {
        if(method!="GET")
            return makeResponse(405,"{\"error\":\"method not allowed\"}");

        return makeResponse(200,api.getGlobalFrames());
    }

    if(path=="/api/page-table")
    {
        if(method!="GET")
            return makeResponse(405,"{\"error\":\"method not allowed\"}");

        std::string pidText=getParameter(query,"pid");

        if(pidText.empty())
            return makeResponse(400,"{\"error\":\"pid is required\"}");

        try
        {
            return makeResponse(200,api.getPageTable(std::stoi(pidText)));
        }
        catch(...)
        {
            return makeResponse(400,"{\"error\":\"invalid pid\"}");
        }
    }

    if(path=="/api/tlb")
    {
        if(method!="GET")
            return makeResponse(405,"{\"error\":\"method not allowed\"}");

        std::string pidText=getParameter(query,"pid");

        if(pidText.empty())
            return makeResponse(400,"{\"error\":\"pid is required\"}");

        try
        {
            return makeResponse(200,api.getTLB(std::stoi(pidText)));
        }
        catch(...)
        {
            return makeResponse(400,"{\"error\":\"invalid pid\"}");
        }
    }

    if(path=="/api/swap")
    {
        if(method!="GET")
            return makeResponse(405,"{\"error\":\"method not allowed\"}");

        std::string pidText=getParameter(query,"pid");

        if(pidText.empty())
            return makeResponse(400,"{\"error\":\"pid is required\"}");

        try
        {
            return makeResponse(200,api.getSwap(std::stoi(pidText)));
        }
        catch(...)
        {
            return makeResponse(400,"{\"error\":\"invalid pid\"}");
        }
    }

    if(path=="/api/create-process")
    {
        std::string name=getParameter(query,"name");
        std::string memoryText=getParameter(query,"memory");
        std::string priorityText=getParameter(query,"priority");
        std::string strategyText=getParameter(query,"strategy");
        std::string burstText=getParameter(query,"burst");

        if(name.empty()||memoryText.empty()||priorityText.empty()||strategyText.empty()||burstText.empty())
            return makeResponse(400,"{\"error\":\"name,memory,priority,strategy,burst are required\"}");

        try
        {
            int memory=std::stoi(memoryText);
            int priority=std::stoi(priorityText);
            int strategy=std::stoi(strategyText);
            int burst=std::stoi(burstText);

            bool success=api.createProcess(name,memory,priority,strategy,burst);
            return makeResponse(success?201:400,boolJson(success));
        }
        catch(...)
        {
            return makeResponse(400,"{\"error\":\"invalid process parameters\"}");
        }
    }

    if(path=="/api/run"||path=="/api/block"||path=="/api/ready"||path=="/api/terminate")
    {
        std::string pidText=getParameter(query,"pid");

        if(pidText.empty())
            return makeResponse(400,"{\"error\":\"pid is required\"}");

        try
        {
            int pid=std::stoi(pidText);
            std::string action;

            if(path=="/api/run")
                action="run";
            else if(path=="/api/block")
                action="block";
            else if(path=="/api/ready")
                action="ready";
            else
                action="terminate";

            bool success=api.processAction(pid,action);
            return makeResponse(success?200:400,boolJson(success));
        }
        catch(...)
        {
            return makeResponse(400,"{\"error\":\"invalid pid\"}");
        }
    }

    if(path=="/api/access")
    {
        std::string pidText=getParameter(query,"pid");
        std::string addressText=getParameter(query,"address");
        std::string typeText=getParameter(query,"type");

        if(pidText.empty()||addressText.empty())
            return makeResponse(400,"{\"error\":\"pid and address are required\"}");

        try
        {
            int pid=std::stoi(pidText);
            int address=std::stoi(addressText);
            bool valid=false;
            AccessType type=parseAccessType(typeText.empty()?"1":typeText,valid);

            if(!valid)
                return makeResponse(400,"{\"error\":\"invalid access type\"}");

            bool success=api.accessMemory(pid,address,type);
            return makeResponse(success?200:400,boolJson(success));
        }
        catch(...)
        {
            return makeResponse(400,"{\"error\":\"invalid access parameters\"}");
        }
    }

    if(path=="/api/replacement")
    {
        std::string pidText=getParameter(query,"pid");
        std::string algorithmText=getParameter(query,"algorithm");

        if(pidText.empty()||algorithmText.empty())
            return makeResponse(400,"{\"error\":\"pid and algorithm are required\"}");

        try
        {
            bool success=api.setReplacementAlgorithm(
                std::stoi(pidText),
                std::stoi(algorithmText)
            );

            return makeResponse(success?200:400,boolJson(success));
        }
        catch(...)
        {
            return makeResponse(400,"{\"error\":\"invalid replacement parameters\"}");
        }
    }

    if(path=="/api/permissions")
    {
        std::string pidText=getParameter(query,"pid");
        std::string readText=getParameter(query,"read");
        std::string writeText=getParameter(query,"write");
        std::string executeText=getParameter(query,"execute");

        if(pidText.empty())
            return makeResponse(400,"{\"error\":\"pid is required\"}");

        try
        {
            bool read=readText=="1"||readText=="true";
            bool write=writeText=="1"||writeText=="true";
            bool execute=executeText=="1"||executeText=="true";

            bool success=api.setPermissions(
                std::stoi(pidText),
                read,
                write,
                execute
            );

            return makeResponse(success?200:400,boolJson(success));
        }
        catch(...)
        {
            return makeResponse(400,"{\"error\":\"invalid permission parameters\"}");
        }
    }

    return makeResponse(404,"{\"error\":\"endpoint not found\"}");
}

bool HttpServer::start()
{
#ifdef _WIN32
    WSADATA data;

    if(WSAStartup(MAKEWORD(2,2),&data)!=0)
        return false;

    SOCKET s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(s==INVALID_SOCKET)
    {
        WSACleanup();
        return false;
    }

    serverSocket=reinterpret_cast<void*>(static_cast<uintptr_t>(s));

    int opt=1;

    setsockopt(
        s,
        SOL_SOCKET,
        SO_REUSEADDR,
        reinterpret_cast<const char*>(&opt),
        sizeof(opt)
    );

    sockaddr_in address{};
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=inet_addr("127.0.0.1");
    address.sin_port=htons(static_cast<unsigned short>(port));

    if(bind(s,reinterpret_cast<sockaddr*>(&address),sizeof(address))==SOCKET_ERROR)
    {
        closesocket(s);
        WSACleanup();
        serverSocket=nullptr;
        return false;
    }

    if(listen(s,10)==SOCKET_ERROR)
    {
        closesocket(s);
        WSACleanup();
        serverSocket=nullptr;
        return false;
    }
#else
    serverSocket=socket(AF_INET,SOCK_STREAM,0);

    if(serverSocket<0)
        return false;

    int opt=1;

    setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    sockaddr_in address{};
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=inet_addr("127.0.0.1");
    address.sin_port=htons(static_cast<unsigned short>(port));

    if(bind(serverSocket,reinterpret_cast<sockaddr*>(&address),sizeof(address))<0)
    {
        close(serverSocket);
        serverSocket=-1;
        return false;
    }

    if(listen(serverSocket,10)<0)
    {
        close(serverSocket);
        serverSocket=-1;
        return false;
    }
#endif

    running=true;

    std::cout<<"HTTP server started at http://127.0.0.1:"<<port<<"\n";
    std::cout<<"API endpoints available under /api/\n";

    while(running)
    {
#ifdef _WIN32
        SOCKET client=accept(s,nullptr,nullptr);

        if(client==INVALID_SOCKET)
        {
            if(!running)
                break;

            continue;
        }
#else
        int client=accept(serverSocket,nullptr,nullptr);

        if(client<0)
        {
            if(!running)
                break;

            continue;
        }
#endif

        char buffer[8192]{};
        int received=recv(client,buffer,sizeof(buffer)-1,0);

        if(received>0)
        {
            std::string request(buffer,received);
            std::istringstream stream(request);

            std::string method;
            std::string target;
            std::string version;

            stream>>method>>target>>version;

            std::string response=handleRequest(method,target);

            send(client,response.c_str(),static_cast<int>(response.size()),0);
        }

#ifdef _WIN32
        closesocket(client);
#else
        close(client);
#endif
    }

    return true;
}

void HttpServer::stop()
{
    if(!running)
        return;

    running=false;

#ifdef _WIN32
    SOCKET s=reinterpret_cast<SOCKET>(serverSocket);

    if(s!=INVALID_SOCKET)
        closesocket(s);

    WSACleanup();
    serverSocket=nullptr;
#else
    if(serverSocket>=0)
    {
        close(serverSocket);
        serverSocket=-1;
    }
#endif
}