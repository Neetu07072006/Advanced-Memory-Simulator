#ifndef REPORTEXPORTER_H
#define REPORTEXPORTER_H

#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cerrno>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include "../process/Process.h"
#include "../memory/MemoryManager.h"
#include "../analytics/Benchmark.h"
#include "../analytics/AllocationBenchmark.h"
#include "SimulationLogger.h"

class ReportExporter
{
    std::string dir;

    bool prepare() const
    {
#ifdef _WIN32
        if(_mkdir(dir.c_str())==0) return true;
        return errno==EEXIST;
#else
        if(mkdir(dir.c_str(),0755)==0) return true;
        return errno==EEXIST;
#endif
    }

public:
    explicit ReportExporter(const std::string& d="reports"):dir(d){}

    bool exportProcessCSV(const std::vector<Process>& p) const
    {
        if(!prepare()) return false;
        std::ofstream out(dir+"/process_report.csv");
        if(!out) return false;

        out<<"PID,Name,MemoryKB,Priority,State,BurstTime,RemainingTime,WaitingTime,TurnaroundTime,CompletionTime,PageAccesses,PageHits,PageFaults,PageReplacements,PageHitRate,PageFaultRate,TLBHits,TLBMisses,SwapIns,SwapOuts,ProtectionViolations\n";
        out<<std::fixed<<std::setprecision(2);

        for(const auto& x:p)
            out<<x.getPid()<<','
               <<x.getName()<<','
               <<x.getMemoryRequired()<<','
               <<x.getPriority()<<','
               <<x.getState()<<','
               <<x.getBurstTime()<<','
               <<x.getRemainingTime()<<','
               <<x.getWaitingTime()<<','
               <<x.getTurnaroundTime()<<','
               <<x.getCompletionTime()<<','
               <<x.getTotalAccesses()<<','
               <<x.getPageHits()<<','
               <<x.getPageFaults()<<','
               <<x.getPageReplacements()<<','
               <<x.getPageHitRate()<<','
               <<x.getPageFaultRate()<<','
               <<x.getTLBHits()<<','
               <<x.getTLBMisses()<<','
               <<x.getSwapIns()<<','
               <<x.getSwapOuts()<<','
               <<x.getProtectionViolations()<<'\n';

        return true;
    }

    bool exportSystemTXT(const std::vector<Process>& p,const MemoryManager& m) const
    {
        if(!prepare()) return false;
        std::ofstream out(dir+"/system_report.txt");
        if(!out) return false;

        long long a=0,h=0,f=0,th=0,tm=0,si=0,so=0,v=0;

        for(const auto& x:p)
        {
            a+=x.getTotalAccesses();
            h+=x.getPageHits();
            f+=x.getPageFaults();
            th+=x.getTLBHits();
            tm+=x.getTLBMisses();
            si+=x.getSwapIns();
            so+=x.getSwapOuts();
            v+=x.getProtectionViolations();
        }

        out<<"ADVANCED MEMORY SIMULATOR SYSTEM REPORT\n";
        out<<"=====================================\n";
        out<<"Processes: "<<p.size()<<"\n";
        out<<"Used Memory KB: "<<m.getUsedMemory()<<"\n";
        out<<"Free Memory KB: "<<m.getFreeMemory()<<"\n";
        out<<"Memory Utilization %: "<<std::fixed<<std::setprecision(2)<<m.getUtilization()<<"\n";
        out<<"Total Accesses: "<<a<<"\n";
        out<<"Page Hits: "<<h<<"\n";
        out<<"Page Faults: "<<f<<"\n";
        out<<"Overall Hit Rate %: "<<(a?100.0*h/a:0.0)<<"\n";
        out<<"Overall Fault Rate %: "<<(a?100.0*f/a:0.0)<<"\n";
        out<<"TLB Hits: "<<th<<"\n";
        out<<"TLB Misses: "<<tm<<"\n";
        out<<"Swap Ins: "<<si<<"\n";
        out<<"Swap Outs: "<<so<<"\n";
        out<<"Protection Violations: "<<v<<"\n";

        return true;
    }

    bool exportEventLog(const SimulationLogger& logger) const
    {
        if(!prepare()) return false;

        std::ofstream out(dir+"/event_log.txt");
        if(!out) return false;

        int i=1;
        for(const auto& e:logger.getEvents())
            out<<i++<<". "<<e<<'\n';

        return true;
    }

    bool exportMemoryCSV(const MemoryManager& m) const
    {
        if(!prepare()) return false;

        std::ofstream out(dir+"/memory_map.csv");
        if(!out) return false;

        out<<"StartKB,SizeKB,Status,PID\n";

        for(const auto& b:m.getSnapshot())
            out<<b.start<<','
               <<b.size<<','
               <<(b.free?"FREE":"USED")<<','
               <<b.processId<<'\n';

        return true;
    }

    bool exportReplacementCSV(const Benchmark& b) const
    {
        if(!prepare()) return false;

        std::ofstream out(dir+"/page_replacement_benchmark.csv");
        if(!out) return false;

        out<<"Algorithm,Hits,Faults,HitRate,FaultRate\n";

        auto write=[&](const char* n,const ReplacementResult& r)
        {
            out<<n<<','
               <<r.hits<<','
               <<r.faults<<','
               <<std::fixed<<std::setprecision(2)
               <<r.hitRate<<','
               <<r.faultRate<<'\n';
        };

        write("FIFO",b.getFIFOResult());
        write("LRU",b.getLRUResult());
        write("Optimal",b.getOptimalResult());

        return true;
    }

    bool exportAllocationCSV(const AllocationBenchmark& b) const
    {
        if(!prepare()) return false;

        std::ofstream out(dir+"/allocation_benchmark.csv");
        if(!out) return false;

        out<<"Algorithm,AllocatedRequests\n";
        out<<"First Fit,"<<b.getFirstFitResult().allocated<<"\n";
        out<<"Best Fit,"<<b.getBestFitResult().allocated<<"\n";
        out<<"Worst Fit,"<<b.getWorstFitResult().allocated<<"\n";
        out<<"Next Fit,"<<b.getNextFitResult().allocated<<"\n";

        return true;
    }

    bool exportAll(const std::vector<Process>& p,
                   const MemoryManager& m,
                   const SimulationLogger& logger,
                   const Benchmark& b,
                   const AllocationBenchmark& ab) const
    {
        return exportProcessCSV(p)
            && exportSystemTXT(p,m)
            && exportEventLog(logger)
            && exportMemoryCSV(m)
            && exportReplacementCSV(b)
            && exportAllocationCSV(ab);
    }

    std::string getDirectory() const
    {
        return dir;
    }
};

#endif