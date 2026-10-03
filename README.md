# Advanced Memory Simulator

Advanced Memory Simulator is a C++ operating-system simulation project that combines contiguous memory allocation, process management, paging, page replacement, TLB caching, swap simulation, memory protection, CPU scheduling, analytics, testing, stress testing, and persistent report export.

## Completed Phases

Phases 1-16 establish the simulator core, allocation strategies, processes, virtual memory, page replacement, TLB, swap, protection, scheduling, analytics, dashboards, logging, testing, and stress testing.

Phase 17 adds persistent TXT and CSV report export.

Phase 18 adds final validation and safe cleanup checks.

Phase 19 packages the project with documentation, build scripts, demonstration flow, and report structure.

Phase 20A introduces a `SimulationController` control layer that centralizes subsystem ownership and application-level operations while keeping `main.cpp` focused on command-line interaction.

## Directory Structure

```text
AdvancedMemorySimulator/
├── main.cpp
├── core/
│   ├── SimulationController.h
│   └── SimulationController.cpp
├── build.bat
├── run.bat
├── memory/
├── allocation/
├── process/
├── virtual_memory/
├── replacement/
├── hardware/
├── protection/
├── scheduler/
├── analytics/
├── output/
└── reports/
```

## Main Features

Physical memory is modeled as 1000 KB and supports First Fit, Best Fit, Worst Fit, and Next Fit.

Each process owns a virtual-memory model with 4 KB pages and three physical frames. FIFO, LRU, and Optimal algorithms are available for page-replacement analysis.

A four-entry TLB and sixteen-slot swap simulation are included for additional memory-system behavior.

Memory protection supports READ, WRITE, and EXECUTE permissions and records protection violations.

CPU scheduling supports FCFS, SJF, Priority, and Round Robin. Memory-aware scheduling, MLFQ, and process event simulation extend the scheduling layer.

Analytics provide process-level and system-level page, TLB, swap, protection, memory, and scheduling information.

Automated tests and stress tests validate core algorithms.

Reports can be exported as TXT and CSV files under `reports/`.

## Phase 20A Architecture Change

The simulator now uses a control layer between the user interface and the OS simulation subsystems:

```text
CLI main.cpp
      |
      v
SimulationController
      |
      +--> ProcessManager
      +--> MemoryManager
      +--> Scheduler
      +--> Virtual Memory and Hardware Models
      +--> Analytics
      +--> Logging / Testing / Reports
```

This structure prepares the simulator for a later web interface without moving the OS simulation logic into the frontend.

## Build

From the project directory on Windows with MinGW g++:

```text
g++ -std=c++17 -O2 main.cpp core/SimulationController.cpp process/ProcessManager.cpp memory/MemoryManager.cpp scheduler/Scheduler.cpp -o AdvancedMemorySimulator.exe
```

Or run `build.bat`.

## Run

```text
AdvancedMemorySimulator.exe
```

Or run `run.bat` after a successful build.

## Recommended Demo

Create two or three processes with different allocation strategies. Access several virtual addresses to produce page hits, faults, TLB activity, and replacement. Display the page table, frames, TLB, swap state, and statistics. Run the replacement benchmark, CPU scheduler, unified dashboard, system tests, stress test, final validation, and report export.

## Reports

The export operation produces:

- `reports/process_report.csv`
- `reports/system_report.txt`
- `reports/memory_map.csv`
- `reports/event_log.txt`

## Scope

This project is a teaching and demonstration simulator. It models operating-system concepts in user-space C++ and is not a kernel or production memory manager.
