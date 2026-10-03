@echo off
g++ -std=c++17 -O2 main.cpp core/SimulationController.cpp process/ProcessManager.cpp memory/MemoryManager.cpp scheduler/Scheduler.cpp -o AdvancedMemorySimulator.exe
if %errorlevel% neq 0 exit /b %errorlevel%
echo Build successful.
