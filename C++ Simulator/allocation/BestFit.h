#ifndef BESTFIT_H
#define BESTFIT_H

#include "AllocationStrategy.h"

class BestFit:public AllocationStrategy
{
public:
    int selectBlock(const std::vector<MemoryBlock>& blocks,int request,int=0) const override
    {
        int index=-1,best=2147483647;
        for(int i=0;i<(int)blocks.size();++i) if(blocks[i].free&&blocks[i].size>=request&&blocks[i].size<best){best=blocks[i].size;index=i;}
        return index;
    }
    std::string getName() const override{return "Best Fit";}
};

#endif
