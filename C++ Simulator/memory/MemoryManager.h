#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include "MemoryBlock.h"
#include "../allocation/AllocationStrategy.h"

class MemoryManager
{
private:
    static const int TOTAL_MEMORY=1000;
    void* memory;
    MemoryBlock* head;
    int nextFitIndex;
    int allocatedBlocks;
    int totalAllocated;
    int findNodeIndex(MemoryBlock* target) const;
    MemoryBlock* getNodeByIndex(int index) const;
    void mergeFreeBlocks();
public:
    MemoryManager();
    ~MemoryManager();
    void initialize();
    bool allocateMemoryForProcess(int processId,int size,AllocationStrategy& strategy);
    bool releaseMemoryForProcess(int processId);
    void displayMemory() const;
    int getTotalMemory() const{return TOTAL_MEMORY;}
    int getUsedMemory() const{return totalAllocated;}
    int getFreeMemory() const{return TOTAL_MEMORY-totalAllocated;}
    int getAllocatedBlocks() const{return allocatedBlocks;}
    double getUtilization() const{return TOTAL_MEMORY?100.0*totalAllocated/TOTAL_MEMORY:0.0;}
    std::vector<MemoryBlock> getSnapshot() const;
};

#endif
