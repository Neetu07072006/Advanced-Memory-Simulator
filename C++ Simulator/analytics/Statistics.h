#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include <iostream>
#include <iomanip>
#include "../process/Process.h"

class Statistics
{
public:
    void displaySystemReport(const std::vector<Process>& p) const
    {
        long long accesses=0,hits=0,faults=0,tlbHits=0,tlbMisses=0,swapIn=0,swapOut=0,violations=0;double avgHit=0;
        for(const auto&x:p){accesses+=x.getTotalAccesses();hits+=x.getPageHits();faults+=x.getPageFaults();tlbHits+=x.getTLBHits();tlbMisses+=x.getTLBMisses();swapIn+=x.getSwapIns();swapOut+=x.getSwapOuts();violations+=x.getProtectionViolations();avgHit+=x.getPageHitRate();}
        avgHit=p.empty()?0:avgHit/p.size();std::cout<<"\n====================================================\nSYSTEM ANALYTICS\n====================================================\nProcesses           : "<<p.size()<<"\nTotal Accesses      : "<<accesses<<"\nPage Hits           : "<<hits<<"\nPage Faults         : "<<faults<<"\nOverall Hit Rate    : "<<std::fixed<<std::setprecision(2)<<(accesses?100.0*hits/accesses:0.0)<<"%\nOverall Fault Rate  : "<<(accesses?100.0*faults/accesses:0.0)<<"%\nTLB Hits            : "<<tlbHits<<"\nTLB Misses          : "<<tlbMisses<<"\nSwap Ins            : "<<swapIn<<"\nSwap Outs           : "<<swapOut<<"\nProtection Violations: "<<violations<<"\nAverage Process Hit : "<<avgHit<<"%\n====================================================\n";
    }
};

#endif
