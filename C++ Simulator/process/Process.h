#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include "../virtual_memory/VirtualMemory.h"
#include "../protection/MemoryProtection.h"
#include "../memory/PhysicalFrameManager.h"

class Process
{
    int pid;
    std::string name;
    int memoryRequired;
    int priority;
    int allocationStrategy;
    int burstTime;
    int remainingTime;
    int waitingTime;
    int turnaroundTime;
    int completionTime;
    int contextSwitches;
    int stateTransitions;
    std::string state;
    std::unique_ptr<VirtualMemory> virtualMemory;
    MemoryProtection protection;
    PhysicalFrameManager* globalFrameManager;
    int globalFrameProcessId;
    std::vector<int> workingSetPages;

    static const int GLOBAL_FRAME_CAPACITY = 3;
    static const int WORKING_SET_WINDOW = 5;
    static const int WORKING_SET_FAULT_THRESHOLD = 60;

public:
    Process(int id,const std::string& n,int memory,int p,int strategy,int burst=5)
        : pid(id),
          name(n),
          memoryRequired(memory),
          priority(p),
          allocationStrategy(strategy),
          burstTime(burst>0?burst:1),
          remainingTime(burst>0?burst:1),
          waitingTime(0),
          turnaroundTime(0),
          completionTime(0),
          contextSwitches(0),
          stateTransitions(0),
          state("NEW"),
          virtualMemory(std::make_unique<VirtualMemory>(memory,3)),
          protection(true,false,false),
          globalFrameManager(nullptr),
          globalFrameProcessId(id),
          workingSetPages()
    {
    }

    Process(const Process&)=delete;
    Process& operator=(const Process&)=delete;

    Process(Process&& other) noexcept
        : pid(other.pid),
          name(std::move(other.name)),
          memoryRequired(other.memoryRequired),
          priority(other.priority),
          allocationStrategy(other.allocationStrategy),
          burstTime(other.burstTime),
          remainingTime(other.remainingTime),
          waitingTime(other.waitingTime),
          turnaroundTime(other.turnaroundTime),
          completionTime(other.completionTime),
          contextSwitches(other.contextSwitches),
          stateTransitions(other.stateTransitions),
          state(std::move(other.state)),
          virtualMemory(std::move(other.virtualMemory)),
          protection(other.protection),
          globalFrameManager(other.globalFrameManager),
          globalFrameProcessId(other.globalFrameProcessId),
          workingSetPages(std::move(other.workingSetPages))
    {
        other.globalFrameManager=nullptr;
    }

    Process& operator=(Process&& other) noexcept
    {
        if(this!=&other)
        {
            pid=other.pid;
            name=std::move(other.name);
            memoryRequired=other.memoryRequired;
            priority=other.priority;
            allocationStrategy=other.allocationStrategy;
            burstTime=other.burstTime;
            remainingTime=other.remainingTime;
            waitingTime=other.waitingTime;
            turnaroundTime=other.turnaroundTime;
            completionTime=other.completionTime;
            contextSwitches=other.contextSwitches;
            stateTransitions=other.stateTransitions;
            state=std::move(other.state);
            virtualMemory=std::move(other.virtualMemory);
            protection=other.protection;
            globalFrameManager=other.globalFrameManager;
            globalFrameProcessId=other.globalFrameProcessId;
            workingSetPages=std::move(other.workingSetPages);
            other.globalFrameManager=nullptr;
        }
        return *this;
    }

    ~Process()=default;

    int getPid() const
    {
        return pid;
    }

    const std::string& getName() const
    {
        return name;
    }

    int getMemoryRequired() const
    {
        return memoryRequired;
    }

    int getVirtualMemorySize() const
    {
        return virtualMemory?virtualMemory->getVirtualMemorySize():0;
    }

    int getPriority() const
    {
        return priority;
    }

    int getAllocationStrategy() const
    {
        return allocationStrategy;
    }

    int getBurstTime() const
    {
        return burstTime;
    }

    int getRemainingTime() const
    {
        return remainingTime;
    }

    int getWaitingTime() const
    {
        return waitingTime;
    }

    int getTurnaroundTime() const
    {
        return turnaroundTime;
    }

    int getCompletionTime() const
    {
        return completionTime;
    }

    int getContextSwitches() const
    {
        return contextSwitches;
    }

    int getStateTransitions() const
    {
        return stateTransitions;
    }

    const std::string& getState() const
    {
        return state;
    }

    void setState(const std::string& value)
    {
        if(state!=value)
        {
            state=value;
            ++stateTransitions;
        }
    }

    void setRemainingTime(int value)
    {
        remainingTime=value<0?0:value;
    }

    void setWaitingTime(int value)
    {
        waitingTime=value<0?0:value;
    }

    void setTurnaroundTime(int value)
    {
        turnaroundTime=value<0?0:value;
    }

    void setCompletionTime(int value)
    {
        completionTime=value<0?0:value;
    }

    void incrementWaitingTime(int value=1)
    {
        if(value>0)
            waitingTime+=value;
    }

    void incrementTurnaroundTime(int value=1)
    {
        if(value>0)
            turnaroundTime+=value;
    }

    void incrementContextSwitches()
    {
        ++contextSwitches;
    }

    void resetScheduling()
    {
        remainingTime=burstTime;
        waitingTime=0;
        turnaroundTime=0;
        completionTime=0;
        contextSwitches=0;
    }

    int getPageHits() const
    {
        return virtualMemory?virtualMemory->getPageHits():0;
    }

    int getPageFaults() const
    {
        return virtualMemory?virtualMemory->getPageFaults():0;
    }

    int getTotalAccesses() const
    {
        return virtualMemory?virtualMemory->getTotalAccesses():0;
    }

    int getPageReplacements() const
    {
        return virtualMemory?virtualMemory->getPageReplacements():0;
    }

    double getPageHitRate() const
    {
        return virtualMemory?virtualMemory->getPageHitRate():0.0;
    }

    double getPageFaultRate() const
    {
        return virtualMemory?virtualMemory->getPageFaultRate():0.0;
    }

    long long getTLBHits() const
    {
        return virtualMemory?virtualMemory->getTLBHits():0;
    }

    long long getTLBMisses() const
    {
        return virtualMemory?virtualMemory->getTLBMisses():0;
    }

    long long getSwapIns() const
    {
        return virtualMemory?virtualMemory->getSwapIns():0;
    }

    long long getSwapOuts() const
    {
        return virtualMemory?virtualMemory->getSwapOuts():0;
    }

    int getProtectionViolations() const
    {
        return protection.getViolations();
    }

    bool canRead() const
    {
        return protection.canRead();
    }

    bool canWrite() const
    {
        return protection.canWrite();
    }

    bool canExecute() const
    {
        return protection.canExecute();
    }

    std::string getProtectionDescription() const
    {
        return protection.getDescription();
    }

    void setPermissions(bool read,bool write,bool execute)
    {
        protection.setPermissions(read,write,execute);
    }

    bool accessMemory(int address,AccessType type=AccessType::READ)
    {
        if(!protection.canAccess(type))
        {
            std::cout<<"\nMEMORY PROTECTION VIOLATION\nPID: "<<pid<<"\n";
            return false;
        }

        if(!virtualMemory)
            return false;

        int physicalAddress=virtualMemory->accessAddress(address);

        if(physicalAddress<0)
            return false;

        int page=virtualMemory->getPageNumber(address);

        if(page>=0)
        {
            auto it=std::find(workingSetPages.begin(),workingSetPages.end(),page);

            if(it!=workingSetPages.end())
                workingSetPages.erase(it);

            workingSetPages.push_back(page);

            while(static_cast<int>(workingSetPages.size())>WORKING_SET_WINDOW)
                workingSetPages.erase(workingSetPages.begin());
        }

        return true;
    }

    bool setReplacementAlgorithm(int algorithm)
    {
        if(!virtualMemory)
            return false;

        if(algorithm<1||algorithm>3)
            return false;

        ReplacementAlgorithm value=
            algorithm==1
                ?ReplacementAlgorithm::FIFO
                :algorithm==2
                    ?ReplacementAlgorithm::LRU
                    :ReplacementAlgorithm::OPTIMAL;

        virtualMemory->setReplacementAlgorithm(value);
        return true;
    }

    void simulateOptimal(const std::vector<int>& references) const
    {
        if(virtualMemory)
            virtualMemory->simulateOptimal(references);
    }

    bool runOptimalBenchmark(const std::vector<int>& references) const
    {
        if(!virtualMemory||references.empty())
            return false;

        virtualMemory->simulateOptimal(references);
        return true;
    }

    void displayPageTable() const
    {
        if(virtualMemory)
            virtualMemory->displayPageTable();
    }

    void displayFrames() const
    {
        if(virtualMemory)
            virtualMemory->displayFrames();
    }

    void displayTLB() const
    {
        if(virtualMemory)
            virtualMemory->displayTLB();
    }

    void displaySwap() const
    {
        if(virtualMemory)
            virtualMemory->displaySwap();
    }

    void displayMemoryStatistics() const
    {
        if(virtualMemory)
            virtualMemory->displayStatistics();
    }

    void displayProtection() const
    {
        std::cout<<"\n";
        std::cout<<"====================================================\n";
        std::cout<<"                MEMORY PROTECTION\n";
        std::cout<<"====================================================\n";
        std::cout<<"PID             : "<<pid<<"\n";
        std::cout<<"READ            : "<<(canRead()?"ENABLED":"DENIED")<<"\n";
        std::cout<<"WRITE           : "<<(canWrite()?"ENABLED":"DENIED")<<"\n";
        std::cout<<"EXECUTE         : "<<(canExecute()?"ENABLED":"DENIED")<<"\n";
        std::cout<<"Violations      : "<<getProtectionViolations()<<"\n";
        std::cout<<"Description     : "<<getProtectionDescription()<<"\n";
        std::cout<<"====================================================\n";
    }

    void attachGlobalFrameManager(PhysicalFrameManager& manager,int processId)
    {
        globalFrameManager=&manager;
        globalFrameProcessId=processId;
    }

    void attachGlobalFrameManager(PhysicalFrameManager* manager,int processId)
    {
        globalFrameManager=manager;
        globalFrameProcessId=processId;
    }

    void attachGlobalFrameManager(PhysicalFrameManager* manager)
    {
        globalFrameManager=manager;
        globalFrameProcessId=pid;
    }

    int getGlobalFrameCapacity() const
    {
        return GLOBAL_FRAME_CAPACITY;
    }

    int getGlobalFrameUsed() const
    {
        if(!globalFrameManager)
            return 0;

        return static_cast<int>(
            globalFrameManager->getProcessFrames(globalFrameProcessId).size()
        );
    }

    int getGlobalFrameFree() const
    {
        int value=GLOBAL_FRAME_CAPACITY-getGlobalFrameUsed();
        return value>0?value:0;
    }

    int getGlobalFrameCount() const
    {
        return getGlobalFrameUsed();
    }

    std::vector<int> getGlobalFrameNumbers() const
    {
        if(!globalFrameManager)
            return {};

        return globalFrameManager->getProcessFrames(globalFrameProcessId);
    }

    void releaseGlobalFrames()
    {
        if(!globalFrameManager)
            return;

        std::vector<int> frames=
            globalFrameManager->getProcessFrames(globalFrameProcessId);

        for(int frame:frames)
            globalFrameManager->releaseFrame(frame);
    }

    int getSwapCapacity() const
    {
        if(!virtualMemory)
            return 0;

        return static_cast<int>(virtualMemory->getSwapSlots().size());
    }

    int getSwapUsed() const
    {
        if(!virtualMemory)
            return 0;

        return static_cast<int>(virtualMemory->getSwapSlots().size());
    }

    int getSwapFree() const
    {
        int value=getSwapCapacity()-getSwapUsed();
        return value>0?value:0;
    }

    std::vector<int> getResidentPages() const
    {
        std::vector<int> pages;

        if(!virtualMemory)
            return pages;

        for(int page=0;page<virtualMemory->getNumberOfPages();++page)
        {
            if(virtualMemory->findFrameForPage(page)>=0)
                pages.push_back(page);
        }

        return pages;
    }

    std::vector<int> getNonResidentPages() const
    {
        std::vector<int> pages;

        if(!virtualMemory)
            return pages;

        for(int page=0;page<virtualMemory->getNumberOfPages();++page)
        {
            if(virtualMemory->findFrameForPage(page)<0)
                pages.push_back(page);
        }

        return pages;
    }

    int getResidentPageCount() const
    {
        return static_cast<int>(getResidentPages().size());
    }

    int getNonResidentPageCount() const
    {
        return static_cast<int>(getNonResidentPages().size());
    }

    int getWorkingSetSize() const
    {
        return static_cast<int>(workingSetPages.size());
    }

    int getWorkingSetWindow() const
    {
        return WORKING_SET_WINDOW;
    }

    int getWorkingSetFaultThreshold() const
    {
        return WORKING_SET_FAULT_THRESHOLD;
    }

    double getWorkingSetFaultRate() const
    {
        if(getTotalAccesses()==0)
            return 0.0;

        return static_cast<double>(getPageFaults())/
               static_cast<double>(getTotalAccesses())*100.0;
    }

    bool isThrashing() const
    {
        return getTotalAccesses()>0 &&
               getWorkingSetFaultRate()>=WORKING_SET_FAULT_THRESHOLD;
    }

    double getMemoryPressure() const
    {
        if(GLOBAL_FRAME_CAPACITY<=0)
            return 0.0;

        return static_cast<double>(getGlobalFrameUsed())/
               GLOBAL_FRAME_CAPACITY*100.0;
    }

    bool exceedsFrames() const
    {
        return getWorkingSetSize()>getGlobalFrameCapacity();
    }

    std::vector<int> getWorkingSetPages() const
    {
        return workingSetPages;
    }

    std::vector<int> getPageTable() const
    {
        if(!virtualMemory)
            return {};

        std::vector<int> table;

        for(int page=0;page<virtualMemory->getNumberOfPages();++page)
            table.push_back(virtualMemory->findFrameForPage(page));

        return table;
    }

    auto getTLBEntries() const
    {
        return virtualMemory->getTLBEntries();
    }

    std::vector<int> getSwapSlots() const
    {
        if(!virtualMemory)
            return {};

        return virtualMemory->getSwapSlots();
    }
};

#endif