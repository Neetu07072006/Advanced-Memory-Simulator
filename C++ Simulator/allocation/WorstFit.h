#ifndef WORSTFIT_H
#define WORSTFIT_H

#include "AllocationStrategy.h"

class WorstFit:public AllocationStrategy
{
public:
    int selectBlock(const std::vector<MemoryBlock>& blocks,int request,int=0) const override
    {
        int index=-1,worst=-1;
        for(int i=0;i<(int)blocks.size();++i) if(blocks[i].free&&blocks[i].size>=request&&blocks[i].size>worst){worst=blocks[i].size;index=i;}
        return index;
    }
    std::string getName() const override{return "Worst Fit";}
};

#endif
