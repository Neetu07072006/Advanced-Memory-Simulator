#ifndef TESTSUITE_H
#define TESTSUITE_H

#include <iostream>
#include <vector>
#include "../replacement/FIFO.h"
#include "../replacement/LRU.h"
#include "../replacement/Optimal.h"
#include "../hardware/TLB.h"
#include "../hardware/SwapManager.h"
#include "../protection/MemoryProtection.h"
#include "../virtual_memory/VirtualMemory.h"

class TestSuite
{
    int passed=0,total=0;
    void check(bool ok,const std::string&name){++total;if(ok){++passed;std::cout<<"[PASS] "<<name<<"\n";}else std::cout<<"[FAIL] "<<name<<"\n";}
public:
    void runAll()
    {
        passed=total=0;std::vector<int> refs={1,2,3,1,4,2,5,1,2,3,4,5};FIFO f(3);LRU l(3);Optimal o(3);f.simulate(refs);l.simulate(refs);o.simulate(refs);check(f.getTotalAccesses()==(int)refs.size(),"FIFO total accesses");check(l.getTotalAccesses()==(int)refs.size(),"LRU total accesses");check(o.getTotalAccesses()==(int)refs.size(),"Optimal total accesses");TLB t(2);t.insert(1,0);check(t.lookup(1)==0,"TLB lookup");SwapManager s(2);check(s.swapOut(1)&&s.swapIn(1),"Swap round trip");MemoryProtection mp;check(mp.canAccess(AccessType::READ)&&!mp.canAccess(AccessType::WRITE),"Memory protection");VirtualMemory vm(16,2);vm.accessAddress(0);vm.accessAddress(0);vm.accessAddress(8);vm.accessAddress(12);check(vm.getPageHits()==1,"Virtual memory page hit");check(vm.getPageFaults()==3,"Virtual memory page faults");check(vm.getPageReplacements()==1,"Virtual memory replacement");std::cout<<"\nTests Passed: "<<passed<<" / "<<total<<"\n";
    }
};

#endif
