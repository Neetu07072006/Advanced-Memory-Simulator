#include "SimulationAPI.h"
#include <sstream>
#include <iomanip>

namespace
{
std::string escapeJson(const std::string& value)
{
    std::string result;

    for(char c:value)
    {
        if(c=='\"')
            result+="\\\"";
        else if(c=='\\')
            result+="\\\\";
        else if(c=='\n')
            result+="\\n";
        else if(c=='\r')
            result+="\\r";
        else if(c=='\t')
            result+="\\t";
        else
            result+=c;
    }

    return result;
}
}

SimulationAPI::SimulationAPI(SimulationController& simulationController)
    :controller(simulationController)
{
}

std::string SimulationAPI::getSystem() const
{
    const auto& processes=controller.getProcesses();
    const auto& memory=controller.getMemoryManager();
    const auto& frames=controller.getGlobalFrameManager();

    int pageFaults=0;

    for(const auto& process:processes)
        pageFaults+=process.getPageFaults();

    double utilization=memory.getUtilization();
    double usedMemory=1000.0*utilization/100.0;
    double freeMemory=1000.0-usedMemory;

    std::ostringstream out;

    out<<"{";
    out<<"\"totalProcesses\":"<<processes.size()<<",";
    out<<"\"runningProcesses\":0,";
    out<<"\"usedMemory\":"<<std::fixed<<std::setprecision(2)<<usedMemory<<",";
    out<<"\"freeMemory\":"<<std::fixed<<std::setprecision(2)<<freeMemory<<",";
    out<<"\"memoryUtilization\":"<<std::fixed<<std::setprecision(2)<<utilization<<",";
    out<<"\"pageFaults\":"<<pageFaults<<",";
    out<<"\"globalFrames\":{";
    out<<"\"total\":"<<frames.getTotalFrames()<<",";
    out<<"\"used\":"<<frames.getUsedFrames()<<",";
    out<<"\"free\":"<<frames.getFreeFrames()<<",";
    out<<"\"utilization\":"<<std::fixed<<std::setprecision(2)<<frames.getUtilization();
    out<<"}";
    out<<"}";

    return out.str();
}

std::string SimulationAPI::getProcesses() const
{
    const auto& processes=controller.getProcessManager().getProcesses();
    std::ostringstream out;

    out<<"{\"processes\":[";

    for(size_t i=0;i<processes.size();++i)
    {
        const auto& p=processes[i];
        std::vector<int> workingSet=p.getWorkingSetPages();

        if(i>0)
            out<<",";

        out<<"{";
        out<<"\"pid\":"<<p.getPid()<<",";
        out<<"\"name\":\""<<p.getName()<<"\",";
        out<<"\"priority\":"<<p.getPriority()<<",";
        out<<"\"memoryRequired\":"<<p.getMemoryRequired()<<",";
        out<<"\"virtualMemorySize\":"<<p.getVirtualMemorySize()<<",";
        out<<"\"state\":\""<<p.getState()<<"\",";
        out<<"\"pageHits\":"<<p.getPageHits()<<",";
        out<<"\"pageFaults\":"<<p.getPageFaults()<<",";
        out<<"\"totalAccesses\":"<<p.getTotalAccesses()<<",";
        out<<"\"pageReplacements\":"<<p.getPageReplacements()<<",";
        out<<"\"pageHitRate\":"<<p.getPageHitRate()<<",";
        out<<"\"pageFaultRate\":"<<p.getPageFaultRate()<<",";
        out<<"\"tlbHits\":"<<p.getTLBHits()<<",";
        out<<"\"tlbMisses\":"<<p.getTLBMisses()<<",";
        out<<"\"swapIns\":"<<p.getSwapIns()<<",";
        out<<"\"swapOuts\":"<<p.getSwapOuts()<<",";
        out<<"\"globalFrameCapacity\":"<<p.getGlobalFrameCapacity()<<",";
        out<<"\"globalFrameUsed\":"<<p.getGlobalFrameUsed()<<",";
        out<<"\"globalFrameFree\":"<<p.getGlobalFrameFree()<<",";
        out<<"\"residentPages\":"<<p.getResidentPageCount()<<",";
        out<<"\"nonResidentPages\":"<<p.getNonResidentPageCount()<<",";
        out<<"\"workingSetSize\":"<<p.getWorkingSetSize()<<",";
        out<<"\"workingSetWindow\":"<<p.getWorkingSetWindow()<<",";
        out<<"\"workingSetFaultRate\":"<<p.getWorkingSetFaultRate()<<",";
        out<<"\"memoryPressure\":"<<p.getMemoryPressure()<<",";
        out<<"\"thrashing\":"<<(p.isThrashing()?"true":"false")<<",";
        out<<"\"workingSetPages\":[";

        for(size_t j=0;j<workingSet.size();++j)
        {
            if(j>0)
                out<<",";

            out<<workingSet[j];
        }

        out<<"]";
        out<<"}";
    }

    out<<"]}";

    return out.str();
}

std::string SimulationAPI::getMemory() const
{
    double utilization=controller.getMemoryManager().getUtilization();

    std::ostringstream out;

    out<<"{";
    out<<"\"totalMemory\":1000,";
    out<<"\"utilization\":"<<std::fixed<<std::setprecision(2)<<utilization<<",";
    out<<"\"blocks\":[]";
    out<<"}";

    return out.str();
}

std::string SimulationAPI::getEvents() const
{
    const auto& events=controller.getLogger().getEvents();
    std::ostringstream out;

    out<<"{\"events\":[";

    for(size_t i=0;i<events.size();++i)
    {
        if(i>0)
            out<<",";

        out<<"\""<<escapeJson(events[i])<<"\"";
    }

    out<<"]}";
    return out.str();
}

std::string SimulationAPI::getPageTable(int pid) const
{
    const auto& processes=controller.getProcesses();

    for(const auto& process:processes)
    {
        if(process.getPid()!=pid)
            continue;

        const auto& table=process.getPageTable();
        std::ostringstream out;

        out<<"{\"pid\":"<<pid<<",\"pages\":[";

        for(size_t i=0;i<table.size();++i)
        {
            if(i>0)
                out<<",";

            out<<"{";
            out<<"\"page\":"<<i<<",";
            out<<"\"frame\":"<<table[i]<<",";
            out<<"\"present\":"<<(table[i]>=0?"true":"false");
            out<<"}";
        }

        out<<"]}";
        return out.str();
    }

    return "{\"error\":\"process not found\"}";
}

std::string SimulationAPI::getTLB(int pid) const
{
    const auto& processes=controller.getProcesses();

    for(const auto& process:processes)
    {
        if(process.getPid()!=pid)
            continue;

        const auto& entries=process.getTLBEntries();
        std::ostringstream out;

        out<<"{\"pid\":"<<pid<<",\"entries\":[";

        for(size_t i=0;i<entries.size();++i)
        {
            if(i>0)
                out<<",";

            out<<"{";
            out<<"\"page\":"<<entries[i].first<<",";
            out<<"\"frame\":"<<entries[i].second;
            out<<"}";
        }

        out<<"]}";
        return out.str();
    }

    return "{\"error\":\"process not found\"}";
}

std::string SimulationAPI::getSwap(int pid) const
{
    const auto& processes=controller.getProcesses();

    for(const auto& process:processes)
    {
        if(process.getPid()!=pid)
            continue;

        const auto& slots=process.getSwapSlots();
        std::ostringstream out;

        out<<"{\"pid\":"<<pid<<",";
        out<<"\"capacity\":"<<process.getSwapCapacity()<<",";
        out<<"\"slots\":[";

        for(size_t i=0;i<slots.size();++i)
        {
            if(i>0)
                out<<",";

            out<<slots[i];
        }

        out<<"]}";
        return out.str();
    }

    return "{\"error\":\"process not found\"}";
}

std::string SimulationAPI::getSchedule() const
{
    const auto& timeline=controller.getSchedulingTimeline();
    std::ostringstream out;

    out<<"{\"algorithm\":"<<controller.getSchedulingAlgorithm()<<",";
    out<<"\"quantum\":"<<controller.getSchedulingQuantum()<<",";
    out<<"\"timeline\":[";

    for(size_t i=0;i<timeline.size();++i)
    {
        if(i>0)
            out<<",";

        out<<"{";
        out<<"\"pid\":"<<timeline[i].pid<<",";
        out<<"\"start\":"<<timeline[i].start<<",";
        out<<"\"duration\":"<<timeline[i].duration<<",";
        out<<"\"end\":"<<timeline[i].end;
        out<<"}";
    }

    out<<"]}";
    return out.str();
}

std::string SimulationAPI::getGlobalFrames() const
{
    const auto& manager=controller.getGlobalFrameManager();
    const auto& frames=manager.getFrames();
    std::ostringstream out;

    out<<"{";
    out<<"\"total\":"<<manager.getTotalFrames()<<",";
    out<<"\"used\":"<<manager.getUsedFrames()<<",";
    out<<"\"free\":"<<manager.getFreeFrames()<<",";
    out<<"\"utilization\":"<<std::fixed<<std::setprecision(2)<<manager.getUtilization()<<",";
    out<<"\"frames\":[";

    for(size_t i=0;i<frames.size();++i)
    {
        if(i>0)
            out<<",";

        const auto& frame=frames[i];

        out<<"{";
        out<<"\"frameNumber\":"<<frame.frameNumber<<",";
        out<<"\"processId\":"<<frame.processId<<",";
        out<<"\"pageNumber\":"<<frame.pageNumber<<",";
        out<<"\"occupied\":"<<(frame.occupied?"true":"false")<<",";
        out<<"\"lastUsed\":"<<frame.lastUsed<<",";
        out<<"\"loadTime\":"<<frame.loadTime;
        out<<"}";
    }

    out<<"]}";
    return out.str();
}

bool SimulationAPI::createProcess(const std::string& name,int memory,int priority,int strategy,int burst)
{
    return controller.createProcess(name,memory,priority,strategy,burst);
}

bool SimulationAPI::processAction(int pid,const std::string& action)
{
    if(action=="run")
        return controller.runProcess(pid);

    if(action=="block")
        return controller.blockProcess(pid);

    if(action=="ready")
        return controller.readyProcess(pid);

    if(action=="terminate")
        return controller.terminateProcess(pid);

    return false;
}

bool SimulationAPI::accessMemory(int pid,int address,AccessType type)
{
    return controller.accessMemory(pid,address,type);
}

void SimulationAPI::runScheduling(int algorithm,int quantum)
{
    controller.runCPUScheduling(algorithm,quantum);
}

bool SimulationAPI::setReplacementAlgorithm(int pid,int algorithm)
{
    return controller.setReplacementAlgorithm(pid,algorithm);
}

bool SimulationAPI::setPermissions(int pid,bool read,bool write,bool execute)
{
    return controller.setPermissions(pid,read,write,execute);
}