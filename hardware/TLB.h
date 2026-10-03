#ifndef TLB_H
#define TLB_H

#include <deque>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>

class TLB
{
    struct Entry{int page;int frame;Entry(int p=-1,int f=-1):page(p),frame(f){}};
    int capacity;
    std::deque<Entry> entries;
    long long hits;
    long long misses;
public:
    TLB(int c=4):capacity(c>0?c:1),hits(0),misses(0){}
    void reset(){entries.clear();hits=0;misses=0;}
    int lookup(int page)
    {
        for(auto it=entries.begin();it!=entries.end();++it) if(it->page==page){int frame=it->frame;entries.erase(it);entries.push_back(Entry(page,frame));++hits;return frame;}
        ++misses;return -1;
    }
    void insert(int page,int frame)
    {
        for(auto it=entries.begin();it!=entries.end();++it) if(it->page==page){entries.erase(it);break;}
        if((int)entries.size()>=capacity) entries.pop_front();
        entries.push_back(Entry(page,frame));
    }
    void invalidatePage(int page){entries.erase(std::remove_if(entries.begin(),entries.end(),[&](const Entry&e){return e.page==page;}),entries.end());}
    long long getHits() const{return hits;}long long getMisses() const{return misses;}int size() const{return capacity;}
    void display() const
    {
        std::cout<<"\n====================================================\n"<<"                    TLB\n"<<"====================================================\n";
        std::cout<<std::left<<std::setw(12)<<"Slot"<<std::setw(12)<<"Page"<<std::setw(12)<<"Frame"<<"\n";
        int i=0;for(const auto&e:entries) std::cout<<std::left<<std::setw(12)<<i++<<std::setw(12)<<e.page<<std::setw(12)<<e.frame<<"\n";
        std::cout<<"Hits    : "<<hits<<"\nMisses  : "<<misses<<"\n====================================================\n";
    }
    std::vector<std::pair<int,int>> getEntries() const
    {
        std::vector<std::pair<int,int>> result;

        for(const auto& entry:entries)
            result.push_back({entry.page,entry.frame});

        return result;
    }
};

#endif
