#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>
#include "../process/Process.h"

struct ScheduleSegment
{
    int pid;
    int start;
    int duration;
    int end;
};

class Scheduler
{
    std::vector<ScheduleSegment> timeline;

    void runFCFS(std::vector<Process>& p);
    void runSJF(std::vector<Process>& p);
    void runPriority(std::vector<Process>& p);
    void runRR(std::vector<Process>& p,int quantum);

public:
    void run(std::vector<Process>& processes,int algorithm,int quantum=2);

    const std::vector<ScheduleSegment>& getTimeline() const;
    void clearTimeline();
};

#endif