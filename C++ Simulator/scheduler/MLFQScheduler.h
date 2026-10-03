#ifndef MLFQSCHEDULER_H
#define MLFQSCHEDULER_H

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include "../process/Process.h"

class MLFQScheduler
{
public:
    void run(std::vector<Process>& p)
    {
        std::queue<int> q0,q1,q2;for(int i=0;i<(int)p.size();++i)if(p[i].getState()!="TERMINATED")q0.push(i);int time=0;
        while(!q0.empty()){int i=q0.front();q0.pop();auto&x=p[i];x.setState("RUNNING");x.incrementContextSwitches();int r=std::min(2,x.getRemainingTime());x.setRemainingTime(x.getRemainingTime()-r);time+=r;if(x.getRemainingTime()>0){x.setState("READY");q1.push(i);}else{x.setCompletionTime(time);x.setTurnaroundTime(time);x.setState("TERMINATED");}}
        while(!q1.empty()){int i=q1.front();q1.pop();auto&x=p[i];x.setState("RUNNING");x.incrementContextSwitches();int r=std::min(4,x.getRemainingTime());x.setRemainingTime(x.getRemainingTime()-r);time+=r;if(x.getRemainingTime()>0){x.setState("READY");q2.push(i);}else{x.setCompletionTime(time);x.setTurnaroundTime(time);x.setState("TERMINATED");}}
        while(!q2.empty()){int i=q2.front();q2.pop();auto&x=p[i];x.setState("RUNNING");int r=x.getRemainingTime();x.setRemainingTime(0);time+=r;x.setCompletionTime(time);x.setTurnaroundTime(time);x.setState("TERMINATED");}
        for(auto&x:p)if(x.getCompletionTime())x.setWaitingTime(x.getTurnaroundTime()-x.getBurstTime());
        std::cout<<"\nMLFQ scheduling completed.\n";
    }
};

#endif
