#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <vector>
#include <string>
#include "../memory/MemoryManager.h"
#include "../memory/PhysicalFrameManager.h"
#include "Process.h"

class ProcessManager
{
    MemoryManager& memoryManager;
    PhysicalFrameManager* globalFrameManager;
    std::vector<Process> processes;
    int nextPid;

    Process* findProcess(int pid);
    const Process* findProcess(int pid) const;

public:
    explicit ProcessManager(
        MemoryManager& manager,
        PhysicalFrameManager* frameManager=nullptr
    ):
        memoryManager(manager),
        globalFrameManager(frameManager),
        nextPid(100)
    {
    }

    bool createProcess(
        const std::string& name,
        int memory,
        int priority,
        int strategy,
        int burst
    );

    bool terminateProcess(int pid);
    bool runProcess(int pid);
    bool blockProcess(int pid);
    bool readyProcess(int pid);

    bool accessMemory(
        int pid,
        int address,
        AccessType type
    );

    bool setReplacementAlgorithm(
        int pid,
        int algorithm
    );

    bool runOptimalBenchmark(
        int pid,
        const std::vector<int>& refs
    );

    bool setPermissions(
        int pid,
        bool r,
        bool w,
        bool x
    );

    bool displayProtection(int pid) const;
    bool displayPageTable(int pid) const;
    bool displayFrames(int pid) const;
    bool displayTLB(int pid) const;
    bool displaySwap(int pid) const;
    bool displayMemoryStatistics(int pid) const;

    void displayProcesses() const;

    std::vector<Process>& getProcesses()
    {
        return processes;
    }

    const std::vector<Process>& getProcesses() const
    {
        return processes;
    }
};

#endif