#ifndef ALLOCATIONBENCHMARK_H
#define ALLOCATIONBENCHMARK_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "../memory/MemoryBlock.h"
#include "../allocation/FirstFit.h"
#include "../allocation/BestFit.h"
#include "../allocation/WorstFit.h"
#include "../allocation/NextFit.h"

struct AllocationResult{int allocated;};

class AllocationBenchmark
{
    int totalMemory=1000;
    AllocationResult first{},best{},worst{},next{};
    int run(const std::vector<int>& reqs,AllocationStrategy& strategy) const
    {
        std::vector<MemoryBlock> b{{0,totalMemory,true,-1}};int allocated=0,nextIndex=0;
        for(int r:reqs)
        {
            if(r<=0) continue;
            int idx=strategy.selectBlock(b,r,nextIndex);
            if(idx<0) continue;
            if(b[idx].size==r) b[idx].free=false;
            else
            {
                MemoryBlock used=b[idx];used.size=r;used.free=false;used.processId=1;
                MemoryBlock rest{b[idx].start+r,b[idx].size-r,true,-1};
                b[idx]=used;b.insert(b.begin()+idx+1,rest);
            }
            ++allocated;nextIndex=idx;
        }
        return allocated;
    }
public:
    void runBenchmark(const std::vector<int>& reqs)
    {
        if(reqs.empty()) return;
        FirstFit f;BestFit b;WorstFit w;NextFit n;
        first={run(reqs,f)};best={run(reqs,b)};worst={run(reqs,w)};next={run(reqs,n)};
        std::cout<<"\n====================================================\nALLOCATION BENCHMARK\n====================================================\nFirst Fit  allocated: "<<first.allocated<<"\nBest Fit   allocated: "<<best.allocated<<"\nWorst Fit  allocated: "<<worst.allocated<<"\nNext Fit   allocated: "<<next.allocated<<"\n====================================================\n";
    }
    AllocationResult getFirstFitResult() const{return first;}
    AllocationResult getBestFitResult() const{return best;}
    AllocationResult getWorstFitResult() const{return worst;}
    AllocationResult getNextFitResult() const{return next;}
};

#endif
