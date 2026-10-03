#ifndef SIMULATIONAPI_H
#define SIMULATIONAPI_H

#include <string>
#include "../core/SimulationController.h"
#include "../process/Process.h"

class SimulationAPI
{
    SimulationController& controller;

public:
    explicit SimulationAPI(SimulationController& simulationController);

    std::string getSystem() const;
    std::string getProcesses() const;
    std::string getMemory() const;
    std::string getEvents() const;
    std::string getPageTable(int pid) const;
    std::string getTLB(int pid) const;
    std::string getSwap(int pid) const;
    std::string getSchedule() const;
    std::string getGlobalFrames() const;

    bool createProcess(const std::string& name,int memory,int priority,int strategy,int burst);
    bool processAction(int pid,const std::string& action);
    bool accessMemory(int pid,int address,AccessType type);
    void runScheduling(int algorithm,int quantum);
    bool setReplacementAlgorithm(int pid,int algorithm);
    bool setPermissions(int pid,bool read,bool write,bool execute);
};

#endif