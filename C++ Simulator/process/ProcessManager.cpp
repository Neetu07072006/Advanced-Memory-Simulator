#include "ProcessManager.h"
#include "../allocation/FirstFit.h"
#include "../allocation/BestFit.h"
#include "../allocation/WorstFit.h"
#include "../allocation/NextFit.h"
#include <iostream>
#include <iomanip>

Process* ProcessManager::findProcess(int pid)
{
    for(auto& p:processes)
        if(p.getPid()==pid)
            return &p;

    return nullptr;
}

const Process* ProcessManager::findProcess(int pid) const
{
    for(const auto& p:processes)
        if(p.getPid()==pid)
            return &p;

    return nullptr;
}

bool ProcessManager::createProcess(
    const std::string& name,
    int memory,
    int priority,
    int strategy,
    int burst
)
{
    if(
        memory<=0 ||
        priority<0 ||
        strategy<1 ||
        strategy>4 ||
        burst<=0
    )
        return false;

    int pid=nextPid;
    bool ok=false;

    if(strategy==1)
    {
        FirstFit s;
        ok=memoryManager.allocateMemoryForProcess(
            pid,
            memory,
            s
        );
    }
    else if(strategy==2)
    {
        BestFit s;
        ok=memoryManager.allocateMemoryForProcess(
            pid,
            memory,
            s
        );
    }
    else if(strategy==3)
    {
        WorstFit s;
        ok=memoryManager.allocateMemoryForProcess(
            pid,
            memory,
            s
        );
    }
    else
    {
        NextFit s;
        ok=memoryManager.allocateMemoryForProcess(
            pid,
            memory,
            s
        );
    }

    if(!ok)
    {
        std::cout
            << "\nMemory allocation failed for process.\n";

        return false;
    }

    processes.emplace_back(
        pid,
        name,
        memory,
        priority,
        strategy,
        burst
    );

    processes.back().attachGlobalFrameManager(
        globalFrameManager,
        pid
    );

    ++nextPid;

    std::cout
        << "\nProcess created successfully. PID: "
        << pid
        << "\n";

    return true;
}

bool ProcessManager::terminateProcess(int pid)
{
    Process* p=findProcess(pid);

    if(!p||p->getState()=="TERMINATED")
        return false;

    p->releaseGlobalFrames();

    memoryManager.releaseMemoryForProcess(pid);

    p->setState("TERMINATED");

    return true;
}

bool ProcessManager::runProcess(int pid)
{
    Process* p=findProcess(pid);

    if(!p||p->getState()=="TERMINATED")
        return false;

    p->setState("RUNNING");

    return true;
}

bool ProcessManager::blockProcess(int pid)
{
    Process* p=findProcess(pid);

    if(!p||p->getState()=="TERMINATED")
        return false;

    p->setState("WAITING");

    return true;
}

bool ProcessManager::readyProcess(int pid)
{
    Process* p=findProcess(pid);

    if(!p||p->getState()=="TERMINATED")
        return false;

    p->setState("READY");

    return true;
}

bool ProcessManager::accessMemory(
    int pid,
    int address,
    AccessType type
)
{
    Process* p=findProcess(pid);

    if(!p||p->getState()=="TERMINATED")
        return false;

    return p->accessMemory(
        address,
        type
    );
}

bool ProcessManager::setReplacementAlgorithm(
    int pid,
    int algorithm
)
{
    Process* p=findProcess(pid);

    if(
        !p ||
        algorithm<1 ||
        algorithm>3
    )
        return false;

    p->setReplacementAlgorithm(algorithm);

    return true;
}

bool ProcessManager::runOptimalBenchmark(
    int pid,
    const std::vector<int>& refs
)
{
    const Process* p=findProcess(pid);

    if(!p||refs.empty())
        return false;

    p->simulateOptimal(refs);

    return true;
}

bool ProcessManager::setPermissions(
    int pid,
    bool r,
    bool w,
    bool x
)
{
    Process* p=findProcess(pid);

    if(!p)
        return false;

    p->setPermissions(
        r,
        w,
        x
    );

    return true;
}

bool ProcessManager::displayProtection(int pid) const
{
    const Process* p=findProcess(pid);

    if(!p)
        return false;

    std::cout
        << "\n====================================================\n"
        << "MEMORY PROTECTION\n"
        << "====================================================\n"
        << "PID         : "
        << p->getPid()
        << "\n"
        << "Permissions : "
        << p->getProtectionDescription()
        << "\n"
        << "Violations  : "
        << p->getProtectionViolations()
        << "\n"
        << "====================================================\n";

    return true;
}

bool ProcessManager::displayPageTable(int pid) const
{
    const Process* p=findProcess(pid);

    if(!p)
        return false;

    p->displayPageTable();

    return true;
}

bool ProcessManager::displayFrames(int pid) const
{
    const Process* p=findProcess(pid);

    if(!p)
        return false;

    p->displayFrames();

    return true;
}

bool ProcessManager::displayTLB(int pid) const
{
    const Process* p=findProcess(pid);

    if(!p)
        return false;

    p->displayTLB();

    return true;
}

bool ProcessManager::displaySwap(int pid) const
{
    const Process* p=findProcess(pid);

    if(!p)
        return false;

    p->displaySwap();

    return true;
}

bool ProcessManager::displayMemoryStatistics(int pid) const
{
    const Process* p=findProcess(pid);

    if(!p)
        return false;

    p->displayMemoryStatistics();

    return true;
}

void ProcessManager::displayProcesses() const
{
    std::cout
        << "\n====================================================\n"
        << "PROCESSES\n"
        << "====================================================\n"
        << std::left
        << std::setw(6)
        << "PID"
        << std::setw(15)
        << "Name"
        << std::setw(10)
        << "Memory"
        << std::setw(10)
        << "Priority"
        << std::setw(12)
        << "Burst"
        << std::setw(14)
        << "State"
        << "\n";

    for(const auto& p:processes)
    {
        std::cout
            << std::left
            << std::setw(6)
            << p.getPid()
            << std::setw(15)
            << p.getName()
            << std::setw(10)
            << p.getMemoryRequired()
            << std::setw(10)
            << p.getPriority()
            << std::setw(12)
            << p.getBurstTime()
            << std::setw(14)
            << p.getState()
            << "\n";
    }

    std::cout
        << "====================================================\n";
}