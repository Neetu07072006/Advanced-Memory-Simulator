#include "SimulationController.h"
#include <iostream>

SimulationController::SimulationController()
    : memoryManager(),
      globalFrameManager(1000,4),
      processManager(memoryManager,&globalFrameManager),
      scheduler(),
      memoryAwareScheduler(),
      mlfqScheduler(),
      eventSimulator(),
      statistics(),
      benchmark(),
      allocationBenchmark(),
      dashboard(),
      stressTest(),
      finalValidation(),
      logger(),
      testSuite(),
      exporter("reports"),
      lastSchedulingAlgorithm(1),
      lastSchedulingQuantum(2)
{
    logger.log("SYSTEM INITIALIZED");
}

MemoryManager& SimulationController::getMemoryManager()
{
    return memoryManager;
}

const MemoryManager& SimulationController::getMemoryManager() const
{
    return memoryManager;
}

PhysicalFrameManager& SimulationController::getGlobalFrameManager()
{
    return globalFrameManager;
}

const PhysicalFrameManager& SimulationController::getGlobalFrameManager() const
{
    return globalFrameManager;
}

ProcessManager& SimulationController::getProcessManager()
{
    return processManager;
}

const ProcessManager& SimulationController::getProcessManager() const
{
    return processManager;
}

const std::vector<Process>& SimulationController::getProcesses() const
{
    return processManager.getProcesses();
}

SimulationLogger& SimulationController::getLogger()
{
    return logger;
}

const SimulationLogger& SimulationController::getLogger() const
{
    return logger;
}

const Benchmark& SimulationController::getBenchmark() const
{
    return benchmark;
}

const AllocationBenchmark& SimulationController::getAllocationBenchmark() const
{
    return allocationBenchmark;
}

const std::vector<SimulationEvent>& SimulationController::getSimulationEvents() const
{
    return eventSimulator.getEvents();
}

int SimulationController::getSimulationTime() const
{
    return eventSimulator.getCurrentTime();
}

bool SimulationController::createProcess(
    const std::string& name,
    int memory,
    int priority,
    int strategy,
    int burst
)
{
    bool ok=processManager.createProcess(
        name,
        memory,
        priority,
        strategy,
        burst
    );

    logger.log(
        ok
            ? "PROCESS CREATED: "+name
            : "PROCESS CREATE FAILED: "+name
    );

    return ok;
}

bool SimulationController::runProcess(int pid)
{
    bool ok=processManager.runProcess(pid);

    logger.log(
        "PROCESS RUN REQUEST: P"+std::to_string(pid)
    );

    return ok;
}

bool SimulationController::blockProcess(int pid)
{
    bool ok=processManager.blockProcess(pid);

    logger.log(
        "PROCESS BLOCK REQUEST: P"+std::to_string(pid)
    );

    return ok;
}

bool SimulationController::readyProcess(int pid)
{
    bool ok=processManager.readyProcess(pid);

    logger.log(
        "PROCESS READY REQUEST: P"+std::to_string(pid)
    );

    return ok;
}

bool SimulationController::terminateProcess(int pid)
{
    bool ok=processManager.terminateProcess(pid);

    logger.log(
        "PROCESS TERMINATE REQUEST: P"+std::to_string(pid)
    );

    return ok;
}

bool SimulationController::accessMemory(
    int pid,
    int address,
    AccessType type
)
{
    bool ok=processManager.accessMemory(
        pid,
        address,
        type
    );

    logger.log(
        "MEMORY ACCESS: P"+
        std::to_string(pid)+
        " ADDRESS "+
        std::to_string(address)
    );

    return ok;
}

bool SimulationController::setReplacementAlgorithm(
    int pid,
    int algorithm
)
{
    bool ok=processManager.setReplacementAlgorithm(
        pid,
        algorithm
    );

    logger.log(
        "PAGE REPLACEMENT ALGORITHM CHANGED: P"+
        std::to_string(pid)
    );

    return ok;
}

bool SimulationController::runOptimalBenchmark(
    int pid,
    const std::vector<int>& references
)
{
    bool ok=processManager.runOptimalBenchmark(
        pid,
        references
    );

    logger.log(
        "OPTIMAL BENCHMARK: P"+
        std::to_string(pid)
    );

    return ok;
}

bool SimulationController::setPermissions(
    int pid,
    bool read,
    bool write,
    bool execute
)
{
    bool ok=processManager.setPermissions(
        pid,
        read,
        write,
        execute
    );

    logger.log(
        "MEMORY PERMISSIONS UPDATED: P"+
        std::to_string(pid)
    );

    return ok;
}

void SimulationController::runCPUScheduling(
    int algorithm,
    int quantum
)
{
    lastSchedulingAlgorithm=algorithm;
    lastSchedulingQuantum=quantum>0?quantum:2;

    scheduler.run(
        processManager.getProcesses(),
        algorithm,
        lastSchedulingQuantum
    );

    logger.log(
        "CPU SCHEDULING EXECUTED"
    );
}

void SimulationController::runMemoryAwareScheduling()
{
    memoryAwareScheduler.run(
        processManager.getProcesses()
    );

    logger.log(
        "MEMORY-AWARE SCHEDULING"
    );
}

void SimulationController::runMLFQ()
{
    mlfqScheduler.run(
        processManager.getProcesses()
    );

    logger.log(
        "MLFQ SCHEDULING"
    );
}

void SimulationController::runEventSimulation()
{
    eventSimulator.run(
        processManager.getProcesses()
    );

    logger.log(
        "PROCESS EVENT SIMULATION"
    );
}

const std::vector<ScheduleSegment>& SimulationController::getSchedulingTimeline() const
{
    return scheduler.getTimeline();
}

int SimulationController::getSchedulingAlgorithm() const
{
    return lastSchedulingAlgorithm;
}

int SimulationController::getSchedulingQuantum() const
{
    return lastSchedulingQuantum;
}

void SimulationController::displayProcesses() const
{
    processManager.displayProcesses();
}

void SimulationController::displaySystemAnalytics() const
{
    statistics.displaySystemReport(
        processManager.getProcesses()
    );
}

void SimulationController::displaySystemDashboard() const
{
    dashboard.display(
        processManager.getProcesses()
    );
}

void SimulationController::displaySystemConfiguration() const
{
    std::cout<<"\n";
    std::cout<<"====================================================\n";
    std::cout<<"              SYSTEM CONFIGURATION\n";
    std::cout<<"====================================================\n";
    std::cout<<"Physical Memory       : 1000 KB\n";
    std::cout<<"Page Size             : 4 KB\n";
    std::cout<<"Global Frames         : 250\n";
    std::cout<<"Frames Per Process    : 3\n";
    std::cout<<"TLB Entries           : 4\n";
    std::cout<<"Swap Slots            : 16\n";
    std::cout<<"Memory Allocation     : First Fit / Best Fit / Worst Fit / Next Fit\n";
    std::cout<<"Page Replacement      : FIFO / LRU / Optimal\n";
    std::cout<<"CPU Scheduling        : FCFS / SJF / Priority / Round Robin\n";
    std::cout<<"Advanced Scheduling   : Memory-Aware / MLFQ\n";
    std::cout<<"Event Simulation      : Enabled\n";
    std::cout<<"Working Set           : Enabled\n";
    std::cout<<"Thrashing Detection   : Enabled\n";
    std::cout<<"Memory Protection     : READ / WRITE / EXECUTE\n";
    std::cout<<"Analytics             : Enabled\n";
    std::cout<<"Fragmentation         : Enabled\n";
    std::cout<<"Event Logging         : Enabled\n";
    std::cout<<"Persistent Reports    : TXT / CSV\n";
    std::cout<<"Validation            : Enabled\n";
    std::cout<<"====================================================\n";
}

void SimulationController::runTests()
{
    testSuite.runAll();

    logger.log(
        "SYSTEM TEST SUITE EXECUTED"
    );
}

void SimulationController::runStressTest(
    int type,
    int first,
    int second,
    int third
)
{
    if(type==1)
    {
        stressTest.runPageReplacementTest(
            first,
            second,
            third
        );

        logger.log(
            "PAGE REPLACEMENT STRESS TEST"
        );
    }
    else if(type==2)
    {
        stressTest.runTLBTest(
            first,
            second
        );

        logger.log(
            "TLB STRESS TEST"
        );
    }
}

bool SimulationController::exportReports()
{
    bool ok=exporter.exportAll(
        processManager.getProcesses(),
        memoryManager,
        logger,
        benchmark,
        allocationBenchmark
    );

    logger.log(
        ok
            ? "REPORT EXPORT COMPLETED"
            : "REPORT EXPORT FAILED"
    );

    return ok;
}

bool SimulationController::runFinalValidation()
{
    bool ok=finalValidation.run(
        processManager.getProcesses(),
        memoryManager
    );

    logger.log(
        ok
            ? "FINAL VALIDATION PASSED"
            : "FINAL VALIDATION FAILED"
    );

    return ok;
}