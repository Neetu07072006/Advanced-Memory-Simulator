#ifndef SIMULATIONLOGGER_H
#define SIMULATIONLOGGER_H

#include <vector>
#include <string>
#include <iostream>

class SimulationLogger
{
    std::vector<std::string> events;
public:
    void log(const std::string& event){events.push_back(event);}
    const std::vector<std::string>& getEvents() const{return events;}
    void display() const{std::cout<<"\n====================================================\nEVENT LOG\n====================================================\n";int i=1;for(const auto&e:events)std::cout<<i++<<". "<<e<<"\n";std::cout<<"====================================================\n";}
    void clear(){events.clear();}
};

#endif
