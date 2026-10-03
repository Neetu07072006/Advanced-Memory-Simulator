#include "Scheduler.h"
#include <algorithm>
#include <iostream>

void Scheduler::clearTimeline()
{
    timeline.clear();
}

const std::vector<ScheduleSegment>& Scheduler::getTimeline() const
{
    return timeline;
}

void Scheduler::run(std::vector<Process>& p,int algorithm,int quantum)
{
    timeline.clear();

    if(p.empty())
        return;

    for(auto& x:p)
        x.resetScheduling();

    if(algorithm==1)
        runFCFS(p);
    else if(algorithm==2)
        runSJF(p);
    else if(algorithm==3)
        runPriority(p);
    else if(algorithm==4)
        runRR(p,quantum>0?quantum:1);
}

void Scheduler::runFCFS(std::vector<Process>& p)
{
    int t=0;

    for(auto& x:p)
    {
        if(x.getState()=="TERMINATED")
            continue;

        x.setState("RUNNING");

        int start=t;
        int duration=x.getBurstTime();

        timeline.push_back({
            x.getPid(),
            start,
            duration,
            start+duration
        });

        t+=duration;

        x.setCompletionTime(t);
        x.setTurnaroundTime(t);
        x.setWaitingTime(start);
        x.setRemainingTime(0);
        x.setState("TERMINATED");
    }
}

void Scheduler::runSJF(std::vector<Process>& p)
{
    std::vector<int> idx;

    for(int i=0;i<(int)p.size();++i)
    {
        if(p[i].getState()!="TERMINATED")
            idx.push_back(i);
    }

    std::sort(
        idx.begin(),
        idx.end(),
        [&](int a,int b)
        {
            if(p[a].getBurstTime()!=p[b].getBurstTime())
                return p[a].getBurstTime()<p[b].getBurstTime();

            return p[a].getPid()<p[b].getPid();
        }
    );

    int t=0;

    for(int i:idx)
    {
        auto& x=p[i];

        x.setState("RUNNING");

        int start=t;
        int duration=x.getBurstTime();

        timeline.push_back({
            x.getPid(),
            start,
            duration,
            start+duration
        });

        t+=duration;

        x.setWaitingTime(start);
        x.setCompletionTime(t);
        x.setTurnaroundTime(t);
        x.setRemainingTime(0);
        x.setState("TERMINATED");
    }
}

void Scheduler::runPriority(std::vector<Process>& p)
{
    std::vector<int> idx;

    for(int i=0;i<(int)p.size();++i)
    {
        if(p[i].getState()!="TERMINATED")
            idx.push_back(i);
    }

    std::sort(
        idx.begin(),
        idx.end(),
        [&](int a,int b)
        {
            if(p[a].getPriority()!=p[b].getPriority())
                return p[a].getPriority()<p[b].getPriority();

            return p[a].getPid()<p[b].getPid();
        }
    );

    int t=0;

    for(int i:idx)
    {
        auto& x=p[i];

        x.setState("RUNNING");

        int start=t;
        int duration=x.getBurstTime();

        timeline.push_back({
            x.getPid(),
            start,
            duration,
            start+duration
        });

        t+=duration;

        x.setWaitingTime(start);
        x.setCompletionTime(t);
        x.setTurnaroundTime(t);
        x.setRemainingTime(0);
        x.setState("TERMINATED");
    }
}

void Scheduler::runRR(std::vector<Process>& p,int quantum)
{
    int t=0;
    bool active=true;

    while(active)
    {
        active=false;

        for(auto& x:p)
        {
            if(x.getState()=="TERMINATED")
                continue;

            if(x.getRemainingTime()<=0)
            {
                x.setState("TERMINATED");
                continue;
            }

            active=true;

            x.setState("RUNNING");
            x.incrementContextSwitches();

            int run=std::min(
                quantum,
                x.getRemainingTime()
            );

            int start=t;

            timeline.push_back({
                x.getPid(),
                start,
                run,
                start+run
            });

            x.setRemainingTime(
                x.getRemainingTime()-run
            );

            t+=run;

            if(x.getRemainingTime()>0)
            {
                x.setState("READY");
            }
            else
            {
                x.setCompletionTime(t);
                x.setTurnaroundTime(t);
                x.setState("TERMINATED");
            }
        }
    }

    for(auto& x:p)
    {
        if(x.getCompletionTime()>0)
        {
            x.setWaitingTime(
                x.getTurnaroundTime()-
                x.getBurstTime()
            );
        }
    }
}