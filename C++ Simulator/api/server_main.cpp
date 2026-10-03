#include <iostream>
#include "SimulationAPI.h"
#include "HttpServer.h"
#include "../core/SimulationController.h"

int main()
{
    SimulationController controller;
    SimulationAPI api(controller);
    HttpServer server(api,8080);

    if(!server.start())
    {
        std::cerr<<"Failed to start HTTP server\n";
        return 1;
    }

    return 0;
}