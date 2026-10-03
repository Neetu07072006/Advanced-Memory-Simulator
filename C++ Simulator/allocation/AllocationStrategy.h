#ifndef ALLOCATIONSTRATEGY_H
#define ALLOCATIONSTRATEGY_H

#include <vector>
#include <string>
#include "../memory/MemoryBlock.h"

class AllocationStrategy
{
public:
    virtual ~AllocationStrategy()=default;
    virtual int selectBlock(const std::vector<MemoryBlock>& blocks,int request,int nextIndex=0) const=0;
    virtual std::string getName() const=0;
};

#endif
