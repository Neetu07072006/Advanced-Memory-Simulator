#ifndef SWAPMANAGER_H
#define SWAPMANAGER_H

#include <set>
#include <iostream>
#include <iomanip>
#include<vector>

class SwapManager
{
    int capacity;
    std::set<int> slots;
    long long swapIns;
    long long swapOuts;
public:
    SwapManager(int c=16):capacity(c>0?c:1),swapIns(0),swapOuts(0){}
    bool contains(int page) const{return slots.count(page)>0;}
    bool swapOut(int page){if((int)slots.size()>=capacity) return false;slots.insert(page);++swapOuts;return true;}
    bool swapIn(int page){auto it=slots.find(page);if(it==slots.end()) return false;slots.erase(it);++swapIns;return true;}
    long long getSwapIns() const{return swapIns;}long long getSwapOuts() const{return swapOuts;}int getUsedSlots() const{return (int)slots.size();}int getCapacity() const{return capacity;}
    void display() const
    {
        std::cout<<"\n====================================================\n"<<"                  SWAP MANAGER\n"<<"====================================================\n";
        std::cout<<"Capacity : "<<capacity<<"\nUsed     : "<<slots.size()<<"\nSwap Ins : "<<swapIns<<"\nSwap Outs: "<<swapOuts<<"\n";
        std::cout<<"Slots    : ";for(int p:slots) std::cout<<p<<' ';std::cout<<"\n====================================================\n";
    }
    std::vector<int> getSlots() const
    {
        return std::vector<int>(slots.begin(),slots.end());
    }
};

#endif
