#ifndef EVENTSIMULATOR_H
#define EVENTSIMULATOR_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include "../process/Process.h"

struct SimulationEvent
{
    int time;
    int pid;
    std::string type;
    std::string detail;
};

class EventSimulator
{
    std::vector<SimulationEvent> events;
    int currentTime;

    void addEvent(int pid,const std::string& type,const std::string& detail)
    {
        events.push_back({currentTime,pid,type,detail});
    }

public:
    EventSimulator():events(),currentTime(0)
    {
    }

    void clear()
    {
        events.clear();
        currentTime=0;
    }

    void run(std::vector<Process>& processes)
    {
        clear();

        for(auto& process:processes)
        {
            int pid=process.getPid();
            int burst=process.getBurstTime();
            int memorySize=process.getVirtualMemorySize();

            addEvent(pid,"PROCESS_ARRIVAL","Process entered simulation");

            process.setState("READY");
            addEvent(pid,"STATE_CHANGE","NEW -> READY");

            process.setState("RUNNING");
            addEvent(pid,"CPU_START","Process started execution");

            int steps=burst>0?burst:1;

            for(int step=0;step<steps;++step)
            {
                addEvent(pid,"CPU_EXECUTION","CPU time consumed");

                if(memorySize>0)
                {
                    int address=(step*4)%memorySize;
                    int faultsBefore=process.getPageFaults();
                    int hitsBefore=process.getPageHits();

                    bool ok=process.accessMemory(address,AccessType::READ);

                    if(!ok)
                    {
                        addEvent(
                            pid,
                            "MEMORY_ACCESS_FAILED",
                            "Virtual address "+std::to_string(address)
                        );
                    }
                    else
                    {
                        addEvent(
                            pid,
                            "MEMORY_ACCESS",
                            "Virtual address "+std::to_string(address)
                        );

                        if(process.getPageFaults()>faultsBefore)
                        {
                            addEvent(
                                pid,
                                "PAGE_FAULT",
                                "Page fault at virtual address "+std::to_string(address)
                            );
                        }
                        else if(process.getPageHits()>hitsBefore)
                        {
                            addEvent(
                                pid,
                                "PAGE_HIT",
                                "Page hit at virtual address "+std::to_string(address)
                            );
                        }
                    }
                }

                ++currentTime;
            }

            process.setRemainingTime(0);
            process.setCompletionTime(currentTime);
            process.setTurnaroundTime(currentTime);
            process.setState("TERMINATED");

            addEvent(pid,"CPU_COMPLETE","Process execution completed");
            addEvent(pid,"STATE_CHANGE","RUNNING -> TERMINATED");
        }
    }

    const std::vector<SimulationEvent>& getEvents() const
    {
        return events;
    }

    int getCurrentTime() const
    {
        return currentTime;
    }

    int getEventCount() const
    {
        return static_cast<int>(events.size());
    }

    void display() const
    {
        std::cout<<"\n";
        std::cout<<"============================================================\n";
        std::cout<<"                 DISCRETE EVENT SIMULATION\n";
        std::cout<<"============================================================\n";

        std::cout<<std::left
                 <<std::setw(8)<<"Time"
                 <<std::setw(8)<<"PID"
                 <<std::setw(24)<<"Event"
                 <<"Detail\n";

        std::cout<<"------------------------------------------------------------\n";

        for(const auto& event:events)
        {
            std::cout<<std::left
                     <<std::setw(8)<<event.time
                     <<std::setw(8)<<event.pid
                     <<std::setw(24)<<event.type
                     <<event.detail<<"\n";
        }

        std::cout<<"------------------------------------------------------------\n";
        std::cout<<"Simulation Time : "<<currentTime<<"\n";
        std::cout<<"Total Events    : "<<events.size()<<"\n";
        std::cout<<"============================================================\n";
    }
};

#endif