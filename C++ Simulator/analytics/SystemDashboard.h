#ifndef SYSTEMDASHBOARD_H
#define SYSTEMDASHBOARD_H

#include <vector>
#include <iostream>
#include <iomanip>
#include "../process/Process.h"

class SystemDashboard
{
public:
    void display(const std::vector<Process>& p) const
    {
        int ready=0,running=0,waiting=0,term=0;long long faults=0,tlbMiss=0,swap=0,viol=0;for(const auto&x:p){if(x.getState()=="READY")++ready;else if(x.getState()=="RUNNING")++running;else if(x.getState()=="WAITING")++waiting;else if(x.getState()=="TERMINATED")++term;faults+=x.getPageFaults();tlbMiss+=x.getTLBMisses();swap+=x.getSwapOuts();viol+=x.getProtectionViolations();}
        std::cout<<"\n====================================================\nUNIFIED SYSTEM DASHBOARD\n====================================================\nProcesses       : "<<p.size()<<"\nREADY           : "<<ready<<"\nRUNNING         : "<<running<<"\nWAITING         : "<<waiting<<"\nTERMINATED      : "<<term<<"\nPage Faults     : "<<faults<<"\nTLB Misses      : "<<tlbMiss<<"\nSwap Outs       : "<<swap<<"\nProtection Hits : "<<viol<<"\n====================================================\n";
    }
    void displayProcessTable(const std::vector<Process>& p) const
    {
        std::cout<<"\n====================================================\nDASHBOARD PROCESS TABLE\n====================================================\n"<<std::left<<std::setw(6)<<"PID"<<std::setw(15)<<"Name"<<std::setw(12)<<"State"<<std::setw(10)<<"Burst"<<std::setw(10)<<"Remain"<<std::setw(10)<<"Faults"<<std::setw(10)<<"TLB"<<"\n";for(const auto&x:p)std::cout<<std::left<<std::setw(6)<<x.getPid()<<std::setw(15)<<x.getName()<<std::setw(12)<<x.getState()<<std::setw(10)<<x.getBurstTime()<<std::setw(10)<<x.getRemainingTime()<<std::setw(10)<<x.getPageFaults()<<std::setw(10)<<x.getTLBMisses()<<"\n";std::cout<<"====================================================\n";
    }
};

#endif
