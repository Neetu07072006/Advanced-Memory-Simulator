#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include "../replacement/FIFO.h"
#include "../replacement/LRU.h"
#include "../replacement/Optimal.h"

struct ReplacementResult{int hits;int faults;double hitRate;double faultRate;};

class Benchmark
{
    ReplacementResult fifoResult{},lruResult{},optimalResult{};
public:
    void runPageReplacement(const std::vector<int>& refs,int frames)
    {
        if(frames<=0||refs.empty()) return;
        FIFO f(frames);LRU l(frames);Optimal o(frames);
        f.simulate(refs);l.simulate(refs);o.simulate(refs);
        fifoResult={f.getHits(),f.getFaults(),f.getHitRate(),f.getFaultRate()};
        lruResult={l.getHits(),l.getFaults(),l.getHitRate(),l.getFaultRate()};
        optimalResult={o.getHits(),o.getFaults(),o.getHitRate(),o.getFaultRate()};
        std::cout<<"\n====================================================\nPAGE REPLACEMENT BENCHMARK\n====================================================\n";
        std::cout<<std::left<<std::setw(12)<<"Algorithm"<<std::setw(10)<<"Hits"<<std::setw(10)<<"Faults"<<std::setw(12)<<"HitRate"<<"\n";
        print("FIFO",fifoResult);print("LRU",lruResult);print("Optimal",optimalResult);
        std::cout<<"====================================================\n";
    }
    void print(const std::string& n,const ReplacementResult& r) const
    {std::cout<<std::left<<std::setw(12)<<n<<std::setw(10)<<r.hits<<std::setw(10)<<r.faults<<std::setw(12)<<std::fixed<<std::setprecision(2)<<r.hitRate<<"%\n";}
    ReplacementResult getFIFOResult() const{return fifoResult;}
    ReplacementResult getLRUResult() const{return lruResult;}
    ReplacementResult getOptimalResult() const{return optimalResult;}
};

#endif
