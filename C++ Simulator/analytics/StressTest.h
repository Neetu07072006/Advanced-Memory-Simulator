#ifndef STRESSTEST_H
#define STRESSTEST_H

#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>
#include "../replacement/FIFO.h"
#include "../replacement/LRU.h"
#include "../replacement/Optimal.h"
#include "../hardware/TLB.h"

class StressTest
{
public:
    void runPageReplacementTest(int references,int pages,int frames) const
    {
        if(references<=0||pages<=0||frames<=0)
        {
            std::cout<<"\nInvalid stress-test parameters.\n";
            return;
        }

        std::mt19937 gen(42);
        std::uniform_int_distribution<int> dist(0,pages-1);
        std::vector<int> refs(references);

        for(int& i:refs)
            i=dist(gen);

        auto run=[&](auto& alg)
        {
            auto s=std::chrono::high_resolution_clock::now();
            alg.simulate(refs);
            auto e=std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double,std::milli>(e-s).count();
        };

        FIFO f(frames);
        LRU l(frames);
        Optimal o(frames);

        double tf=run(f);
        double tl=run(l);
        double to=run(o);

        std::cout<<"\n====================================================\n";
        std::cout<<"PAGE REPLACEMENT STRESS TEST\n";
        std::cout<<"====================================================\n";
        std::cout<<"References : "<<references<<"\n";
        std::cout<<"Pages      : "<<pages<<"\n";
        std::cout<<"Frames     : "<<frames<<"\n";
        std::cout<<"FIFO       : "<<tf<<" ms\n";
        std::cout<<"LRU        : "<<tl<<" ms\n";
        std::cout<<"Optimal    : "<<to<<" ms\n";
        std::cout<<"====================================================\n";
    }

    void runTLBTest(int accesses,int pages) const
    {
        if(accesses<=0||pages<=0)
        {
            std::cout<<"\nInvalid stress-test parameters.\n";
            return;
        }

        TLB tlb(4);
        std::mt19937 gen(42);
        std::uniform_int_distribution<int> dist(0,pages-1);

        auto s=std::chrono::high_resolution_clock::now();

        for(int i=0;i<accesses;++i)
        {
            int p=dist(gen);
            int f=tlb.lookup(p);

            if(f==-1)
                tlb.insert(p,p%4);
        }

        auto e=std::chrono::high_resolution_clock::now();

        std::cout<<"\n====================================================\n";
        std::cout<<"TLB STRESS TEST\n";
        std::cout<<"====================================================\n";
        std::cout<<"Accesses : "<<accesses<<"\n";
        std::cout<<"Pages    : "<<pages<<"\n";
        std::cout<<"Hits     : "<<tlb.getHits()<<"\n";
        std::cout<<"Misses   : "<<tlb.getMisses()<<"\n";
        std::cout<<"Time     : "<<std::chrono::duration<double,std::milli>(e-s).count()<<" ms\n";
        std::cout<<"====================================================\n";
    }
};

#endif