#include "MemoryManager.h"
#include <cstdlib>
#include <algorithm>

MemoryManager::MemoryManager():memory(nullptr),head(nullptr),nextFitIndex(0),allocatedBlocks(0),totalAllocated(0){}

MemoryManager::~MemoryManager()
{
    MemoryBlock* cur=head;
    while(cur){MemoryBlock* n=cur->next;delete cur;cur=n;}
    if(memory) std::free(memory);
}

void MemoryManager::initialize()
{
    if(head){MemoryBlock* cur=head;while(cur){MemoryBlock* n=cur->next;delete cur;cur=n;}head=nullptr;}
    if(!memory) memory=std::malloc(TOTAL_MEMORY*1024);
    head=new MemoryBlock(0,TOTAL_MEMORY,true,-1);
    nextFitIndex=0;
    allocatedBlocks=0;
    totalAllocated=0;
}

int MemoryManager::findNodeIndex(MemoryBlock* target) const
{
    int i=0;for(MemoryBlock* cur=head;cur;cur=cur->next,++i) if(cur==target) return i;return -1;
}

MemoryBlock* MemoryManager::getNodeByIndex(int index) const
{
    int i=0;for(MemoryBlock* cur=head;cur;cur=cur->next,++i) if(i==index) return cur;return nullptr;
}

std::vector<MemoryBlock> MemoryManager::getSnapshot() const
{
    std::vector<MemoryBlock> out;for(MemoryBlock* cur=head;cur;cur=cur->next) out.emplace_back(cur->start,cur->size,cur->free,cur->processId);return out;
}

bool MemoryManager::allocateMemoryForProcess(int processId,int size,AllocationStrategy& strategy)
{
    if(size<=0||size>TOTAL_MEMORY||!head) return false;
    std::vector<MemoryBlock> snap=getSnapshot();
    int current=nextFitIndex;
    int index=strategy.selectBlock(snap,size,current);
    if(index<0) return false;
    MemoryBlock* block=getNodeByIndex(index);
    if(!block||!block->free||block->size<size) return false;
    if(block->size==size)
    {
        block->free=false;block->processId=processId;
    }
    else
    {
        MemoryBlock* used=new MemoryBlock(block->start,size,false,processId);
        MemoryBlock* rest=new MemoryBlock(block->start+size,block->size-size,true,-1);
        used->prev=block->prev;used->next=rest;rest->prev=used;rest->next=block->next;
        if(used->prev) used->prev->next=used; else head=used;
        if(rest->next) rest->next->prev=rest;
        delete block;
    }
    MemoryBlock* chosen=getNodeByIndex(index);
    nextFitIndex=findNodeIndex(chosen?chosen:head);
    ++allocatedBlocks;totalAllocated+=size;
    return true;
}

void MemoryManager::mergeFreeBlocks()
{
    MemoryBlock* cur=head;
    while(cur&&cur->next)
    {
        if(cur->free&&cur->next->free)
        {
            MemoryBlock* n=cur->next;cur->size+=n->size;cur->next=n->next;if(n->next) n->next->prev=cur;delete n;
        }
        else cur=cur->next;
    }
}

bool MemoryManager::releaseMemoryForProcess(int processId)
{
    bool released=false;MemoryBlock* cur=head;
    while(cur)
    {
        if(!cur->free&&cur->processId==processId)
        {
            totalAllocated-=cur->size;allocatedBlocks=std::max(0,allocatedBlocks-1);cur->free=true;cur->processId=-1;released=true;
        }
        cur=cur->next;
    }
    mergeFreeBlocks();
    if(released) nextFitIndex=0;
    return released;
}

void MemoryManager::displayMemory() const
{
    std::cout<<"\n====================================================\n";
    std::cout<<"               PHYSICAL MEMORY MAP\n";
    std::cout<<"====================================================\n";
    std::cout<<std::left<<std::setw(12)<<"Start"<<std::setw(12)<<"Size(KB)"<<std::setw(15)<<"Status"<<std::setw(12)<<"PID"<<"\n";
    std::cout<<"----------------------------------------------------\n";
    for(MemoryBlock* cur=head;cur;cur=cur->next) std::cout<<std::left<<std::setw(12)<<cur->start<<std::setw(12)<<cur->size<<std::setw(15)<<(cur->free?"FREE":"USED")<<std::setw(12)<<(cur->free?"-":std::to_string(cur->processId))<<"\n";
    std::cout<<"----------------------------------------------------\n";
    std::cout<<"Used Memory   : "<<totalAllocated<<" KB\n";
    std::cout<<"Free Memory   : "<<getFreeMemory()<<" KB\n";
    std::cout<<"Utilization   : "<<std::fixed<<std::setprecision(2)<<getUtilization()<<"%\n";
    std::cout<<"====================================================\n";
}
