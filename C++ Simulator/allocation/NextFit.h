#ifndef NEXTFIT_H
#define NEXTFIT_H

#include "AllocationStrategy.h"

class NextFit:public AllocationStrategy
{
public:
    int selectBlock(const std::vector<MemoryBlock>& blocks,int request,int nextIndex=0) const override
    {
        if(blocks.empty()) return -1;
        int n=(int)blocks.size();
        int start=((nextIndex%n)+n)%n;
        for(int step=0;step<n;++step){int i=(start+step)%n;if(blocks[i].free&&blocks[i].size>=request) return i;}
        return -1;
    }
    std::string getName() const override{return "Next Fit";}
};

#endif
