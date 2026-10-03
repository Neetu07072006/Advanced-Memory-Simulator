#ifndef FIRSTFIT_H
#define FIRSTFIT_H

#include "AllocationStrategy.h"

class FirstFit:public AllocationStrategy
{
public:
    int selectBlock(const std::vector<MemoryBlock>& blocks,int request,int=0) const override
    {
        for(int i=0;i<(int)blocks.size();++i) if(blocks[i].free&&blocks[i].size>=request) return i;
        return -1;
    }
    std::string getName() const override{return "First Fit";}
};

#endif
