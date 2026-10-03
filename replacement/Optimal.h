#ifndef OPTIMAL_H
#define OPTIMAL_H

#include <vector>
#include <algorithm>
#include <climits>

class Optimal
{
    int capacity;
    int hits;
    int faults;
    int total;
public:
    Optimal(int c=3):capacity(c>0?c:1),hits(0),faults(0),total(0){}
    void simulate(const std::vector<int>& refs)
    {
        hits=faults=total=0;std::vector<int> frames;
        for(size_t i=0;i<refs.size();++i){++total;int page=refs[i];if(std::find(frames.begin(),frames.end(),page)!=frames.end()){++hits;continue;}++faults;if((int)frames.size()<capacity){frames.push_back(page);continue;}int victimIndex=-1,farthest=-1;for(int j=0;j<(int)frames.size();++j){int next=INT_MAX;for(size_t k=i+1;k<refs.size();++k) if(refs[k]==frames[j]){next=(int)k;break;}if(next>farthest){farthest=next;victimIndex=j;}}frames[victimIndex]=page;}
    }
    int getHits() const{return hits;}int getFaults() const{return faults;}int getTotalAccesses() const{return total;}
    double getHitRate() const{return total?100.0*hits/total:0.0;}double getFaultRate() const{return total?100.0*faults/total:0.0;}
};

#endif
