#ifndef LRU_H
#define LRU_H

#include <vector>
#include <list>
#include <algorithm>

class LRU
{
    int capacity;
    std::list<int> pages;
    int hits;
    int faults;
    int total;
public:
    LRU(int c=3):capacity(c>0?c:1),hits(0),faults(0),total(0){}
    LRU(const LRU&)=default;LRU& operator=(const LRU&)=default;LRU(LRU&&)=default;LRU& operator=(LRU&&)=default;
    void reset(){pages.clear();hits=0;faults=0;total=0;}
    int accessPage(int page)
    {
        ++total;auto it=std::find(pages.begin(),pages.end(),page);if(it!=pages.end()){++hits;pages.erase(it);pages.push_back(page);return -1;}
        ++faults;int victim=-1;if((int)pages.size()>=capacity){victim=pages.front();pages.pop_front();}pages.push_back(page);return victim;
    }
    void simulate(const std::vector<int>& refs){reset();for(int p:refs) accessPage(p);}
    int getHits() const{return hits;}int getFaults() const{return faults;}int getTotalAccesses() const{return total;}
    double getHitRate() const{return total?100.0*hits/total:0.0;}double getFaultRate() const{return total?100.0*faults/total:0.0;}
};

#endif
