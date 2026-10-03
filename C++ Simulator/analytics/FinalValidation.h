#ifndef FINALVALIDATION_H
#define FINALVALIDATION_H

#include <vector>
#include <iostream>
#include <set>
#include <string>
#include <algorithm>
#include "../process/Process.h"
#include "../memory/MemoryManager.h"
#include "../replacement/FIFO.h"
#include "../replacement/LRU.h"
#include "../replacement/Optimal.h"

class FinalValidation
{
    bool uniquePids(const std::vector<Process>& p) const{std::set<int>s;for(const auto&x:p)if(!s.insert(x.getPid()).second)return false;return true;}
    bool memoryConsistent(const MemoryManager& m) const{return m.getUsedMemory()>=0&&m.getFreeMemory()>=0&&m.getUsedMemory()<=m.getTotalMemory();}
public:
    bool run(const std::vector<Process>& p,const MemoryManager& m) const
    {
        int passed=0,total=0;auto check=[&](bool ok,const std::string&n){++total;if(ok){++passed;std::cout<<"[PASS] "<<n<<"\n";}else std::cout<<"[FAIL] "<<n<<"\n";};
        check(memoryConsistent(m),"Physical memory bounds");check(uniquePids(p),"Unique process IDs");check(std::all_of(p.begin(),p.end(),[](const Process&x){return x.getMemoryRequired()>0&&x.getBurstTime()>0;}),"Process parameter validation");
        std::vector<int> refs={0,1,2,0,1,3,0,2,1,3};FIFO f(3);LRU l(3);Optimal o(3);f.simulate(refs);l.simulate(refs);o.simulate(refs);check(f.getHits()+f.getFaults()==f.getTotalAccesses(),"FIFO accounting");check(l.getHits()+l.getFaults()==l.getTotalAccesses(),"LRU accounting");check(o.getHits()+o.getFaults()==o.getTotalAccesses(),"Optimal accounting");
        std::cout<<"\nFinal Validation: "<<passed<<" / "<<total<<" checks passed.\n";return passed==total;
    }
};

#endif
