#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

struct MemoryBlock
{
    int start;
    int size;
    bool free;
    int processId;
    MemoryBlock* prev;
    MemoryBlock* next;
    MemoryBlock(int s=0,int z=0,bool f=true,int p=-1):start(s),size(z),free(f),processId(p),prev(nullptr),next(nullptr){}
};

#endif
