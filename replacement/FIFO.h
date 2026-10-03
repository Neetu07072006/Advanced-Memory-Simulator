#ifndef FIFO_H
#define FIFO_H

#include <vector>
#include <deque>
#include <algorithm>

class FIFO
{
    int capacity;
    std::deque<int> pages;
    int hits;
    int faults;
    int total;
public:
    FIFO(int c=3):capacity(c>0?c:1),hits(0),faults(0),total(0){}
    FIFO(const FIFO&)=default;
    FIFO& operator=(const FIFO&)=default;
    FIFO(FIFO&&)=default;
    FIFO& operator=(FIFO&&)=default;
    void reset(){pages.clear();hits=0;faults=0;total=0;}
    int accessPage(int page)
    {
        ++total;
        if(std::find(pages.begin(),pages.end(),page)!=pages.end()){++hits;return -1;}
        ++faults;int victim=-1;if((int)pages.size()>=capacity){victim=pages.front();pages.pop_front();}pages.push_back(page);return victim;
    }
    void simulate(const std::vector<int>& refs)
    {reset();for(int p:refs) accessPage(p);}
    int getHits() const{return hits;}int getFaults() const{return faults;}int getTotalAccesses() const{return total;}
    double getHitRate() const{return total?100.0*hits/total:0.0;}double getFaultRate() const{return total?100.0*faults/total:0.0;}
};

#endif
