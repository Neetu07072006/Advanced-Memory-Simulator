#ifndef VIRTUALMEMORY_H
#define VIRTUALMEMORY_H

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <utility>
#include <algorithm>
#include "../replacement/FIFO.h"
#include "../replacement/LRU.h"
#include "../replacement/Optimal.h"
#include "../hardware/TLB.h"
#include "../hardware/SwapManager.h"
#include "../memory/WorkingSetManager.h"
#include "PagingManager.h"

enum class ReplacementAlgorithm
{
    FIFO,
    LRU,
    OPTIMAL
};

class VirtualMemory
{
    static const int PAGE_SIZE=4;
    int virtualMemorySize;
    int numberOfPages;
    int numberOfFrames;
    std::vector<int> pageTable;
    std::vector<bool> frameUsed;
    std::vector<int> framePages;
    FIFO fifo;
    LRU lru;
    TLB tlb;
    SwapManager swap;
    WorkingSetManager workingSet;
    PagingManager pagingManager;
    PhysicalFrameManager* globalFrameManager;
    int ownerProcessId;
    ReplacementAlgorithm replacementAlgorithm;
    int pageHits;
    int pageFaults;
    int totalAccesses;
    int pageReplacements;
    int tlbHits;
    int tlbMisses;
    int swapIns;
    int swapOuts;

    void syncViews()
    {
        std::fill(pageTable.begin(),pageTable.end(),-1);
        std::fill(frameUsed.begin(),frameUsed.end(),false);
        std::fill(framePages.begin(),framePages.end(),-1);

        for(int page=0;page<numberOfPages;++page)
        {
            int frame=pagingManager.getFrameNumber(page);

            if(frame>=0)
                pageTable[page]=frame;
        }

        std::vector<int> residentPages=pagingManager.getResidentPages();

        for(size_t i=0;i<residentPages.size();++i)
        {
            int page=residentPages[i];

            if(page<0||page>=numberOfPages)
                continue;

            int frame=pagingManager.getFrameNumber(page);

            if(frame<0||frame>=numberOfFrames)
                continue;

            frameUsed[frame]=true;
            framePages[frame]=page;
        }
    }

public:
    VirtualMemory(int memorySize,int physicalFrames=3)
        :virtualMemorySize(memorySize>0?memorySize:PAGE_SIZE),
         numberOfPages(0),
         numberOfFrames(physicalFrames>0?physicalFrames:1),
         pageTable(),
         frameUsed(),
         framePages(),
         fifo(physicalFrames>0?physicalFrames:1),
         lru(physicalFrames>0?physicalFrames:1),
         tlb(4),
         swap(16),
         workingSet(5,60),
         pagingManager(),
         globalFrameManager(nullptr),
         ownerProcessId(-1),
         replacementAlgorithm(ReplacementAlgorithm::FIFO),
         pageHits(0),
         pageFaults(0),
         totalAccesses(0),
         pageReplacements(0),
         tlbHits(0),
         tlbMisses(0),
         swapIns(0),
         swapOuts(0)
    {
        numberOfPages=(virtualMemorySize+PAGE_SIZE-1)/PAGE_SIZE;
        pageTable.assign(numberOfPages,-1);
        frameUsed.assign(numberOfFrames,false);
        framePages.assign(numberOfFrames,-1);
        pagingManager.resize(numberOfPages);
    }

    VirtualMemory(const VirtualMemory&)=delete;
    VirtualMemory& operator=(const VirtualMemory&)=delete;

    VirtualMemory(VirtualMemory&& other) noexcept
        :virtualMemorySize(other.virtualMemorySize),
         numberOfPages(other.numberOfPages),
         numberOfFrames(other.numberOfFrames),
         pageTable(std::move(other.pageTable)),
         frameUsed(std::move(other.frameUsed)),
         framePages(std::move(other.framePages)),
         fifo(std::move(other.fifo)),
         lru(std::move(other.lru)),
         tlb(std::move(other.tlb)),
         swap(std::move(other.swap)),
         workingSet(std::move(other.workingSet)),
         pagingManager(std::move(other.pagingManager)),
         globalFrameManager(other.globalFrameManager),
         ownerProcessId(other.ownerProcessId),
         replacementAlgorithm(other.replacementAlgorithm),
         pageHits(other.pageHits),
         pageFaults(other.pageFaults),
         totalAccesses(other.totalAccesses),
         pageReplacements(other.pageReplacements),
         tlbHits(other.tlbHits),
         tlbMisses(other.tlbMisses),
         swapIns(other.swapIns),
         swapOuts(other.swapOuts)
    {
        other.globalFrameManager=nullptr;
        other.ownerProcessId=-1;
    }

    VirtualMemory& operator=(VirtualMemory&& other) noexcept
    {
        if(this!=&other)
        {
            releaseGlobalFrames();

            virtualMemorySize=other.virtualMemorySize;
            numberOfPages=other.numberOfPages;
            numberOfFrames=other.numberOfFrames;
            pageTable=std::move(other.pageTable);
            frameUsed=std::move(other.frameUsed);
            framePages=std::move(other.framePages);
            fifo=std::move(other.fifo);
            lru=std::move(other.lru);
            tlb=std::move(other.tlb);
            swap=std::move(other.swap);
            workingSet=std::move(other.workingSet);
            pagingManager=std::move(other.pagingManager);
            globalFrameManager=other.globalFrameManager;
            ownerProcessId=other.ownerProcessId;
            replacementAlgorithm=other.replacementAlgorithm;
            pageHits=other.pageHits;
            pageFaults=other.pageFaults;
            totalAccesses=other.totalAccesses;
            pageReplacements=other.pageReplacements;
            tlbHits=other.tlbHits;
            tlbMisses=other.tlbMisses;
            swapIns=other.swapIns;
            swapOuts=other.swapOuts;

            other.globalFrameManager=nullptr;
            other.ownerProcessId=-1;
        }

        return *this;
    }

    ~VirtualMemory()
    {
        releaseGlobalFrames();
    }

    int getVirtualMemorySize() const
    {
        return virtualMemorySize;
    }

    int getNumberOfPages() const
    {
        return numberOfPages;
    }

    int getPageSize() const
    {
        return PAGE_SIZE;
    }

    int getNumberOfFrames() const
    {
        return numberOfFrames;
    }

    int getPageNumber(int virtualAddress) const
    {
        if(virtualAddress<0||virtualAddress>=virtualMemorySize)
            return -1;

        return virtualAddress/PAGE_SIZE;
    }

    int getOffset(int virtualAddress) const
    {
        if(virtualAddress<0||virtualAddress>=virtualMemorySize)
            return -1;

        return virtualAddress%PAGE_SIZE;
    }

    void attachGlobalFrameManager(PhysicalFrameManager* manager,int pid)
    {
        globalFrameManager=manager;
        ownerProcessId=pid;
        pagingManager.attachFrameManager(manager,pid);
        syncViews();
    }

    void releaseGlobalFrames()
    {
        if(globalFrameManager)
        {
            pagingManager.releaseAll();
            globalFrameManager=nullptr;
            ownerProcessId=-1;
        }
    }

    int getGlobalFrameCapacity() const
    {
        return globalFrameManager?globalFrameManager->getTotalFrames():0;
    }

    int getGlobalFrameUsed() const
    {
        return globalFrameManager?globalFrameManager->getUsedFrames():0;
    }

    int getGlobalFrameFree() const
    {
        return globalFrameManager?globalFrameManager->getFreeFrames():0;
    }

    int getResidentPageCount() const
    {
        return pagingManager.getResidentPageCount();
    }

    int getNonResidentPageCount() const
    {
        return pagingManager.getNonResidentPageCount();
    }

    bool isPagePresent(int page) const
    {
        return pagingManager.isPagePresent(page);
    }

    int findFrameForPage(int page) const
    {
        return pagingManager.getFrameNumber(page);
    }

    int getFreeFrame() const
    {
        for(int i=0;i<numberOfFrames;++i)
        {
            if(!frameUsed[i])
                return i;
        }

        return -1;
    }

    void setReplacementAlgorithm(ReplacementAlgorithm algorithm)
    {
        pagingManager.releaseAll();

        replacementAlgorithm=algorithm;

        fifo.reset();
        lru.reset();
        tlb.reset();
        workingSet.reset();

        pageHits=0;
        pageFaults=0;
        totalAccesses=0;
        pageReplacements=0;
        tlbHits=0;
        tlbMisses=0;
        swapIns=0;
        swapOuts=0;

        std::fill(pageTable.begin(),pageTable.end(),-1);
        std::fill(frameUsed.begin(),frameUsed.end(),false);
        std::fill(framePages.begin(),framePages.end(),-1);
    }

    std::string getReplacementAlgorithmName() const
    {
        if(replacementAlgorithm==ReplacementAlgorithm::FIFO)
            return "FIFO";

        if(replacementAlgorithm==ReplacementAlgorithm::LRU)
            return "LRU";

        return "OPTIMAL";
    }

    int loadPage(int page)
    {
        if(page<0||page>=numberOfPages)
            return -1;

        if(pagingManager.isPagePresent(page))
        {
            pagingManager.accessPage(page);
            syncViews();
            return pagingManager.getFrameNumber(page);
        }

        if(!globalFrameManager)
            return -1;

        if(getResidentPageCount()>=numberOfFrames)
        {
            if(replacementAlgorithm==ReplacementAlgorithm::OPTIMAL)
                return -1;

            int victim=-1;

            if(replacementAlgorithm==ReplacementAlgorithm::FIFO)
                victim=fifo.accessPage(page);
            else if(replacementAlgorithm==ReplacementAlgorithm::LRU)
                victim=lru.accessPage(page);

            if(victim<0)
                return -1;

            int victimFrame=pagingManager.getFrameNumber(victim);

            if(victimFrame<0)
                return -1;

            swap.swapOut(victim);
            ++swapOuts;

            tlb.invalidatePage(victim);

            if(!pagingManager.unloadPage(victim))
                return -1;

            ++pageReplacements;
        }

        int frame=pagingManager.loadPage(page);

        if(frame<0)
            return -1;

        if(swap.contains(page))
        {
            swap.swapIn(page);
            ++swapIns;
        }

        syncViews();

        return frame;
    }

    int replacePage(int page)
    {
        return loadPage(page);
    }

    int accessAddress(int virtualAddress)
    {
        ++totalAccesses;

        if(virtualAddress<0||virtualAddress>=virtualMemorySize)
        {
            std::cout<<"\nInvalid virtual address!\n";
            return -1;
        }

        int page=getPageNumber(virtualAddress);
        int offset=getOffset(virtualAddress);

        int frame=tlb.lookup(page);

        if(frame>=0&&pagingManager.isPagePresent(page))
        {
            ++tlbHits;
            ++pageHits;

            pagingManager.accessPage(page);

            if(replacementAlgorithm==ReplacementAlgorithm::LRU)
                lru.accessPage(page);

            workingSet.recordReference(page,false);

            syncViews();

            return frame*PAGE_SIZE+offset;
        }

        ++tlbMisses;

        if(pagingManager.isPagePresent(page))
        {
            ++pageHits;

            frame=pagingManager.getFrameNumber(page);

            pagingManager.accessPage(page);

            if(replacementAlgorithm==ReplacementAlgorithm::LRU)
                lru.accessPage(page);

            tlb.insert(page,frame);

            workingSet.recordReference(page,false);

            syncViews();

            return frame*PAGE_SIZE+offset;
        }

        ++pageFaults;

        frame=loadPage(page);

        workingSet.recordReference(page,true);

        if(frame<0)
        {
            std::cout<<"\nPAGE FAULT\n";
            std::cout<<"Unable to load page "<<page<<"\n";
            return -1;
        }

        tlb.insert(page,frame);

        syncViews();

        return frame*PAGE_SIZE+offset;
    }

    void simulateOptimal(const std::vector<int>& referenceString) const
    {
        Optimal optimal(numberOfFrames);
        optimal.simulate(referenceString);

        std::cout<<"\n";
        std::cout<<"====================================================\n";
        std::cout<<"             OPTIMAL PAGE REPLACEMENT\n";
        std::cout<<"====================================================\n";
        std::cout<<"Frames          : "<<numberOfFrames<<"\n";
        std::cout<<"Total Accesses  : "<<optimal.getTotalAccesses()<<"\n";
        std::cout<<"Page Hits       : "<<optimal.getHits()<<"\n";
        std::cout<<"Page Faults     : "<<optimal.getFaults()<<"\n";
        std::cout<<std::fixed<<std::setprecision(2);
        std::cout<<"Hit Rate        : "<<optimal.getHitRate()<<"%\n";
        std::cout<<"Fault Rate      : "<<optimal.getFaultRate()<<"%\n";
        std::cout<<"====================================================\n";
    }

    void displayPageTable() const
    {
        std::cout<<"\n";
        std::cout<<"====================================================\n";
        std::cout<<"                    PAGE TABLE\n";
        std::cout<<"====================================================\n";
        std::cout<<std::left<<std::setw(12)<<"Page"
                 <<std::setw(15)<<"Frame"
                 <<std::setw(15)<<"Status"<<"\n";
        std::cout<<"----------------------------------------------------\n";

        for(int page=0;page<numberOfPages;++page)
        {
            std::cout<<std::left<<std::setw(12)<<page;

            if(pageTable[page]<0)
            {
                std::cout<<std::setw(15)<<"-"
                         <<std::setw(15)<<"NOT PRESENT";
            }
            else
            {
                std::cout<<std::setw(15)<<pageTable[page]
                         <<std::setw(15)<<"PRESENT";
            }

            std::cout<<"\n";
        }

        std::cout<<"====================================================\n";
    }

    void displayFrames() const
    {
        std::cout<<"\n";
        std::cout<<"====================================================\n";
        std::cout<<"                  PHYSICAL FRAMES\n";
        std::cout<<"====================================================\n";
        std::cout<<std::left<<std::setw(15)<<"Frame"
                 <<std::setw(15)<<"Page"
                 <<std::setw(15)<<"Status"<<"\n";
        std::cout<<"----------------------------------------------------\n";

        for(int frame=0;frame<numberOfFrames;++frame)
        {
            std::cout<<std::left<<std::setw(15)<<frame;

            if(!frameUsed[frame])
            {
                std::cout<<std::setw(15)<<"-"
                         <<std::setw(15)<<"FREE";
            }
            else
            {
                std::cout<<std::setw(15)<<framePages[frame]
                         <<std::setw(15)<<"OCCUPIED";
            }

            std::cout<<"\n";
        }

        std::cout<<"====================================================\n";
    }

    void displayTLB() const
    {
        std::cout<<"\n";
        std::cout<<"====================================================\n";
        std::cout<<"                  TLB STATUS\n";
        std::cout<<"====================================================\n";
        std::cout<<"TLB Capacity     : "<<getTLBCapacity()<<"\n";
        std::cout<<"TLB Hits         : "<<tlbHits<<"\n";
        std::cout<<"TLB Misses       : "<<tlbMisses<<"\n";

        double rate=0.0;

        if(tlbHits+tlbMisses>0)
            rate=static_cast<double>(tlbHits)/(tlbHits+tlbMisses)*100.0;

        std::cout<<std::fixed<<std::setprecision(2);
        std::cout<<"TLB Hit Rate     : "<<rate<<"%\n";
        std::cout<<"====================================================\n";
    }

    void displaySwap() const
    {
        std::cout<<"\n";
        std::cout<<"====================================================\n";
        std::cout<<"                    SWAP STATUS\n";
        std::cout<<"====================================================\n";
        std::cout<<"Swap Capacity    : "<<getSwapCapacity()<<"\n";
        std::cout<<"Swap Ins         : "<<swapIns<<"\n";
        std::cout<<"Swap Outs        : "<<swapOuts<<"\n";
        std::cout<<"====================================================\n";
    }

    void displayWorkingSet() const
    {
        std::cout<<"\n";
        std::cout<<"====================================================\n";
        std::cout<<"                  WORKING SET STATUS\n";
        std::cout<<"====================================================\n";
        std::cout<<"Window Size       : "<<workingSet.getWindowSize()<<"\n";
        std::cout<<"Working Set Size  : "<<workingSet.getWorkingSetSize()<<"\n";
        std::cout<<"Total References  : "<<workingSet.getTotalReferences()<<"\n";
        std::cout<<"Page Faults       : "<<workingSet.getPageFaults()<<"\n";
        std::cout<<std::fixed<<std::setprecision(2);
        std::cout<<"Fault Rate        : "<<workingSet.getPageFaultRate()<<"%\n";
        std::cout<<"Thrashing         : "<<(workingSet.isThrashing()?"YES":"NO")<<"\n";
        std::cout<<"Working Set       : ";

        std::vector<int> pages=workingSet.getWorkingSet();

        for(int page:pages)
            std::cout<<page<<" ";

        std::cout<<"\n";
        std::cout<<"====================================================\n";
    }

    void displayStatistics() const
    {
        std::cout<<"\n";
        std::cout<<"====================================================\n";
        std::cout<<"              VIRTUAL MEMORY STATISTICS\n";
        std::cout<<"====================================================\n";
        std::cout<<"Virtual Memory Size : "<<virtualMemorySize<<" KB\n";
        std::cout<<"Page Size           : "<<PAGE_SIZE<<" KB\n";
        std::cout<<"Virtual Pages       : "<<numberOfPages<<"\n";
        std::cout<<"Physical Frames     : "<<numberOfFrames<<"\n";
        std::cout<<"Replacement         : "<<getReplacementAlgorithmName()<<"\n";
        std::cout<<"Total Accesses      : "<<totalAccesses<<"\n";
        std::cout<<"Page Hits           : "<<pageHits<<"\n";
        std::cout<<"Page Faults         : "<<pageFaults<<"\n";
        std::cout<<"Page Replacements   : "<<pageReplacements<<"\n";
        std::cout<<"TLB Hits            : "<<tlbHits<<"\n";
        std::cout<<"TLB Misses          : "<<tlbMisses<<"\n";
        std::cout<<"Swap Ins            : "<<swapIns<<"\n";
        std::cout<<"Swap Outs           : "<<swapOuts<<"\n";
        std::cout<<"Working Set Size    : "<<workingSet.getWorkingSetSize()<<"\n";
        std::cout<<"Thrashing           : "<<(workingSet.isThrashing()?"YES":"NO")<<"\n";

        double hitRate=0.0;
        double faultRate=0.0;

        if(totalAccesses>0)
        {
            hitRate=static_cast<double>(pageHits)/totalAccesses*100.0;
            faultRate=static_cast<double>(pageFaults)/totalAccesses*100.0;
        }

        std::cout<<std::fixed<<std::setprecision(2);
        std::cout<<"Page Hit Rate       : "<<hitRate<<"%\n";
        std::cout<<"Page Fault Rate     : "<<faultRate<<"%\n";
        std::cout<<"====================================================\n";
    }

    int getPageHits() const
    {
        return pageHits;
    }

    int getPageFaults() const
    {
        return pageFaults;
    }

    int getTotalAccesses() const
    {
        return totalAccesses;
    }

    int getPageReplacements() const
    {
        return pageReplacements;
    }

    double getPageHitRate() const
    {
        return totalAccesses>0
            ?static_cast<double>(pageHits)/totalAccesses*100.0
            :0.0;
    }

    double getPageFaultRate() const
    {
        return totalAccesses>0
            ?static_cast<double>(pageFaults)/totalAccesses*100.0
            :0.0;
    }

    long long getTLBHits() const
    {
        return tlbHits;
    }

    long long getTLBMisses() const
    {
        return tlbMisses;
    }

    long long getSwapIns() const
    {
        return swapIns;
    }

    long long getSwapOuts() const
    {
        return swapOuts;
    }

    int getTLBCapacity() const
    {
        return 4;
    }

    int getSwapCapacity() const
    {
        return 16;
    }

    int getWorkingSetSize() const
    {
        return workingSet.getWorkingSetSize();
    }

    int getWorkingSetWindow() const
    {
        return workingSet.getWindowSize();
    }

    double getWorkingSetFaultRate() const
    {
        return workingSet.getPageFaultRate();
    }

    bool isThrashing() const
    {
        return workingSet.isThrashing();
    }

    double getMemoryPressure() const
    {
        return workingSet.getMemoryPressure(numberOfFrames);
    }

    std::vector<int> getWorkingSetPages() const
    {
        return workingSet.getWorkingSet();
    }

    std::vector<int> getPageTable() const
    {
        return pageTable;
    }

    std::vector<bool> getFrameUsed() const
    {
        return frameUsed;
    }

    std::vector<int> getFramePages() const
    {
        return framePages;
    }

    auto getTLBEntries() const
    {
        return tlb.getEntries();
    }

    auto getSwapSlots() const
    {
        return swap.getSlots();
    }

    int accessPage(int page)
    {
        if(page<0||page>=numberOfPages)
            return -1;

        return accessAddress(page*PAGE_SIZE);
    }

    bool writePage(int page)
    {
        if(page<0||page>=numberOfPages)
            return false;

        return accessAddress(page*PAGE_SIZE)>=0;
    }

    bool isResident(int page) const
    {
        return pagingManager.isPagePresent(page);
    }

    int findFreeGlobalFrame() const
    {
        return pagingManager.findFreeGlobalFrame();
    }

    void clearReferences()
    {
        pagingManager.clearReferences();
    }
};

#endif