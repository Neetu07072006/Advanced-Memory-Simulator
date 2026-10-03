#ifndef WORKINGSETMANAGER_H
#define WORKINGSETMANAGER_H

#include <vector>
#include <deque>
#include <set>
#include <algorithm>
#include <iostream>

class WorkingSetManager
{
    int windowSize;
    std::deque<int> recentPages;
    std::set<int> workingSet;
    int totalReferences;
    int pageFaults;
    int threshold;

    void rebuildWorkingSet()
    {
        workingSet.clear();

        for(int page:recentPages)
            workingSet.insert(page);
    }

public:
    explicit WorkingSetManager(int window=5,int faultThreshold=60)
        :windowSize(window>0?window:1),
         recentPages(),
         workingSet(),
         totalReferences(0),
         pageFaults(0),
         threshold(faultThreshold>0?faultThreshold:1)
    {
    }

    void reset()
    {
        recentPages.clear();
        workingSet.clear();
        totalReferences=0;
        pageFaults=0;
    }

    void setWindowSize(int window)
    {
        windowSize=window>0?window:1;

        while(static_cast<int>(recentPages.size())>windowSize)
            recentPages.pop_front();

        rebuildWorkingSet();
    }

    void setFaultThreshold(int value)
    {
        threshold=value>0?value:1;
    }

    int getWindowSize() const
    {
        return windowSize;
    }

    int getFaultThreshold() const
    {
        return threshold;
    }

    void recordReference(int page,bool fault)
    {
        ++totalReferences;

        if(fault)
            ++pageFaults;

        recentPages.push_back(page);

        while(static_cast<int>(recentPages.size())>windowSize)
            recentPages.pop_front();

        rebuildWorkingSet();
    }

    const std::deque<int>& getRecentPages() const
    {
        return recentPages;
    }

    std::vector<int> getWorkingSet() const
    {
        return std::vector<int>(workingSet.begin(),workingSet.end());
    }

    int getWorkingSetSize() const
    {
        return static_cast<int>(workingSet.size());
    }

    int getTotalReferences() const
    {
        return totalReferences;
    }

    int getPageFaults() const
    {
        return pageFaults;
    }

    double getPageFaultRate() const
    {
        if(totalReferences==0)
            return 0.0;

        return static_cast<double>(pageFaults)
            / static_cast<double>(totalReferences)
            * 100.0;
    }

    bool isThrashing() const
    {
        return totalReferences>0&&getPageFaultRate()>=threshold;
    }

    double getMemoryPressure(int allocatedFrames) const
    {
        if(allocatedFrames<=0)
            return 0.0;

        double pressure=
            static_cast<double>(workingSet.size())
            / static_cast<double>(allocatedFrames)
            * 100.0;

        return std::min(pressure,100.0);
    }

    bool exceedsFrames(int allocatedFrames) const
    {
        return allocatedFrames>0&&
               static_cast<int>(workingSet.size())>allocatedFrames;
    }

    void display() const
    {
        std::cout<<"\n";
        std::cout<<"====================================================\n";
        std::cout<<"                  WORKING SET\n";
        std::cout<<"====================================================\n";
        std::cout<<"Window Size       : "<<windowSize<<"\n";
        std::cout<<"Working Set Size  : "<<workingSet.size()<<"\n";
        std::cout<<"Total References  : "<<totalReferences<<"\n";
        std::cout<<"Page Faults       : "<<pageFaults<<"\n";
        std::cout<<"Fault Rate        : "<<getPageFaultRate()<<"%\n";
        std::cout<<"Thrashing         : "<<(isThrashing()?"YES":"NO")<<"\n";
        std::cout<<"Recent Pages      : ";

        for(size_t i=0;i<recentPages.size();++i)
        {
            std::cout<<recentPages[i];

            if(i+1<recentPages.size())
                std::cout<<" ";
        }

        std::cout<<"\nWorking Set       : ";

        for(int page:workingSet)
            std::cout<<page<<" ";

        std::cout<<"\n";
        std::cout<<"====================================================\n";
    }
};

#endif