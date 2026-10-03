#ifndef SIMULATIONCONTROLLER_H
#define SIMULATIONCONTROLLER_H

#include <string>
#include <vector>
#include "../memory/MemoryManager.h"
#include "../memory/PhysicalFrameManager.h"
#include "../process/ProcessManager.h"
#include "../scheduler/Scheduler.h"
#include "../scheduler/MemoryAwareScheduler.h"
#include "../scheduler/MLFQScheduler.h"
#include "../scheduler/EventSimulator.h"
#include "../analytics/Statistics.h"
#include "../analytics/Benchmark.h"
#include "../analytics/AllocationBenchmark.h"
#include "../analytics/SystemDashboard.h"
#include "../analytics/StressTest.h"
#include "../analytics/FinalValidation.h"
#include "../output/SimulationLogger.h"
#include "../output/TestSuite.h"
#include "../output/ReportExporter.h"

class SimulationController
{
    MemoryManager memoryManager;
    PhysicalFrameManager globalFrameManager;
    ProcessManager processManager;
    Scheduler scheduler;
    MemoryAwareScheduler memoryAwareScheduler;
    MLFQScheduler mlfqScheduler;
    EventSimulator eventSimulator;
    Statistics statistics;
    Benchmark benchmark;
    AllocationBenchmark allocationBenchmark;
    SystemDashboard dashboard;
    StressTest stressTest;
    FinalValidation finalValidation;
    SimulationLogger logger;
    TestSuite testSuite;
    ReportExporter exporter;
    int lastSchedulingAlgorithm;
    int lastSchedulingQuantum;

public:
    SimulationController();

    MemoryManager& getMemoryManager();
    const MemoryManager& getMemoryManager() const;

    PhysicalFrameManager& getGlobalFrameManager();
    const PhysicalFrameManager& getGlobalFrameManager() const;

    ProcessManager& getProcessManager();
    const ProcessManager& getProcessManager() const;

    const std::vector<Process>& getProcesses() const;

    SimulationLogger& getLogger();
    const SimulationLogger& getLogger() const;

    const Benchmark& getBenchmark() const;
    const AllocationBenchmark& getAllocationBenchmark() const;

    const std::vector<SimulationEvent>& getSimulationEvents() const;
    int getSimulationTime() const;

    bool createProcess(
        const std::string& name,
        int memory,
        int priority,
        int strategy,
        int burst
    );

    bool runProcess(int pid);
    bool blockProcess(int pid);
    bool readyProcess(int pid);
    bool terminateProcess(int pid);

    bool accessMemory(
        int pid,
        int address,
        AccessType type = AccessType::READ
    );

    bool setReplacementAlgorithm(
        int pid,
        int algorithm
    );

    bool runOptimalBenchmark(
        int pid,
        const std::vector<int>& references
    );

    bool setPermissions(
        int pid,
        bool read,
        bool write,
        bool execute
    );

    void runCPUScheduling(
        int algorithm,
        int quantum = 2
    );

    void runMemoryAwareScheduling();
    void runMLFQ();
    void runEventSimulation();

    const std::vector<ScheduleSegment>& getSchedulingTimeline() const;
    int getSchedulingAlgorithm() const;
    int getSchedulingQuantum() const;

    void displayProcesses() const;
    void displaySystemAnalytics() const;
    void displaySystemDashboard() const;
    void displaySystemConfiguration() const;

    void runTests();
    void runStressTest(
        int type,
        int first,
        int second,
        int third = 0
    );

    bool exportReports();
    bool runFinalValidation();
};

#endif