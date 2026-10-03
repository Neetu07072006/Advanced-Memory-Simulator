#ifndef MEMORYAWARESCHEDULER_H
#define MEMORYAWARESCHEDULER_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "../process/Process.h"

class MemoryAwareScheduler
{
public:
    void run(std::vector<Process>& processes)
    {
        std::vector<int> idx;for(int i=0;i<(int)processes.size();++i)if(processes[i].getState()!="TERMINATED")idx.push_back(i);
        std::sort(idx.begin(),idx.end(),[&](int a,int b){double sa=processes[a].getPriority()+processes[a].getPageFaultRate()+processes[a].getTLBMisses()-processes[a].getTLBHits();double sb=processes[b].getPriority()+processes[b].getPageFaultRate()+processes[b].getTLBMisses()-processes[b].getTLBHits();return sa<sb;});
        std::cout<<"\n====================================================\nMEMORY-AWARE SCHEDULING\n====================================================\n";int rank=1;for(int i:idx)std::cout<<rank++<<". PID "<<processes[i].getPid()<<"  "<<processes[i].getName()<<"\n";std::cout<<"====================================================\n";
    }
};

#endif
