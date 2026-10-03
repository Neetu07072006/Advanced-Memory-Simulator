#include <iostream>
#include <string>
#include <vector>
#include "core/SimulationController.h"

using namespace std;

void displayMainMenu()
{
    cout<<"\n============================================\n       ADVANCED MEMORY SIMULATOR\n============================================\n";
    cout<<"1. Create Process\n2. Display Processes\n3. Run Process\n4. Block Process\n5. Ready Process\n6. Access Virtual Memory\n7. Display Page Table\n8. Display Physical Frames\n9. Display TLB\n10. Display Swap\n11. Display Memory Statistics\n12. Set Page Replacement Algorithm\n13. Run Optimal Benchmark\n14. CPU Scheduling\n15. Set Memory Permissions\n16. Access Memory with Permission\n17. Display Protection\n18. Terminate Process\n19. Display Physical Memory\n20. System Analytics\n21. Page Replacement Benchmark\n22. Allocation Benchmark\n23. Memory-Aware Scheduling\n24. Multilevel Feedback Queue\n25. Process Event Simulation\n26. Unified System Dashboard\n27. Dashboard Process Table\n28. System Configuration\n29. Run System Tests\n30. Display Event Log\n31. Clear Event Log\n32. Stress Test\n33. Export Reports\n34. Final Validation\n35. Exit\n============================================\nEnter choice: ";
}

int main()
{
    SimulationController simulator;
    int choice=0;
    do
    {
        displayMainMenu();
        cin>>choice;
        if(!cin){cin.clear();cin.ignore(10000,'\n');choice=0;}
        if(choice==1)
        {
            string name;int memory,priority,strategy,burst;
            cout<<"\nProcess name: ";cin>>name;
            cout<<"Memory required (KB): ";cin>>memory;
            cout<<"Priority: ";cin>>priority;
            cout<<"Burst Time: ";cin>>burst;
            cout<<"\n1. First Fit\n2. Best Fit\n3. Worst Fit\n4. Next Fit\nStrategy: ";cin>>strategy;
            cout<<(simulator.createProcess(name,memory,priority,strategy,burst)?"\nProcess created successfully.\n":"\nProcess creation failed.\n");
        }
        else if(choice==2) simulator.displayProcesses();
        else if(choice==3){int pid;cout<<"\nPID: ";cin>>pid;cout<<(simulator.runProcess(pid)?"\nProcess running.\n":"\nProcess run failed.\n");}
        else if(choice==4){int pid;cout<<"\nPID: ";cin>>pid;cout<<(simulator.blockProcess(pid)?"\nProcess blocked.\n":"\nProcess block failed.\n");}
        else if(choice==5){int pid;cout<<"\nPID: ";cin>>pid;cout<<(simulator.readyProcess(pid)?"\nProcess ready.\n":"\nProcess ready failed.\n");}
        else if(choice==6){int pid,address;cout<<"\nPID: ";cin>>pid;cout<<"Virtual Address: ";cin>>address;cout<<(simulator.accessMemory(pid,address,AccessType::READ)?"\nAccess successful.\n":"\nAccess failed.\n");}
        else if(choice==7){int pid;cout<<"\nPID: ";cin>>pid;simulator.getProcessManager().displayPageTable(pid);}
        else if(choice==8){int pid;cout<<"\nPID: ";cin>>pid;simulator.getProcessManager().displayFrames(pid);}
        else if(choice==9){int pid;cout<<"\nPID: ";cin>>pid;simulator.getProcessManager().displayTLB(pid);}
        else if(choice==10){int pid;cout<<"\nPID: ";cin>>pid;simulator.getProcessManager().displaySwap(pid);}
        else if(choice==11){int pid;cout<<"\nPID: ";cin>>pid;simulator.getProcessManager().displayMemoryStatistics(pid);}
        else if(choice==12){int pid,algorithm;cout<<"\nPID: ";cin>>pid;cout<<"1. FIFO\n2. LRU\n3. Optimal\nAlgorithm: ";cin>>algorithm;cout<<(simulator.setReplacementAlgorithm(pid,algorithm)?"\nReplacement algorithm updated.\n":"\nUpdate failed.\n");}
        else if(choice==13)
        {
            int pid,count;cout<<"\nPID: ";cin>>pid;cout<<"Number of page references: ";cin>>count;
            if(count<=0){cout<<"\nInvalid reference count.\n";continue;}
            vector<int> refs(count);cout<<"Reference string: ";for(int&i:refs)cin>>i;
            cout<<(simulator.runOptimalBenchmark(pid,refs)?"\nOptimal benchmark completed.\n":"\nOptimal benchmark failed.\n");
        }
        else if(choice==14)
        {
            int algorithm,quantum=2;cout<<"\n1. FCFS\n2. SJF\n3. Priority\n4. Round Robin\nAlgorithm: ";cin>>algorithm;
            if(algorithm==4){cout<<"Time Quantum: ";cin>>quantum;}
            simulator.runCPUScheduling(algorithm,quantum);
        }
        else if(choice==15)
        {
            int pid,r,w,x;cout<<"\nPID: ";cin>>pid;cout<<"Read permission (1/0): ";cin>>r;cout<<"Write permission (1/0): ";cin>>w;cout<<"Execute permission (1/0): ";cin>>x;
            cout<<(simulator.setPermissions(pid,r,w,x)?"\nPermissions updated.\n":"\nPermission update failed.\n");
        }
        else if(choice==16)
        {
            int pid,address,type;cout<<"\nPID: ";cin>>pid;cout<<"Virtual Address: ";cin>>address;cout<<"1. READ\n2. WRITE\n3. EXECUTE\nAccess Type: ";cin>>type;
            if(type<1||type>3){cout<<"\nInvalid access type.\n";continue;}
            AccessType a=type==1?AccessType::READ:type==2?AccessType::WRITE:AccessType::EXECUTE;
            cout<<(simulator.accessMemory(pid,address,a)?"\nProtected access successful.\n":"\nProtected access denied.\n");
        }
        else if(choice==17){int pid;cout<<"\nPID: ";cin>>pid;simulator.getProcessManager().displayProtection(pid);}
        else if(choice==18){int pid;cout<<"\nPID: ";cin>>pid;cout<<(simulator.terminateProcess(pid)?"\nProcess terminated and memory released.\n":"\nTermination failed.\n");}
        else if(choice==19) simulator.displayPhysicalMemory();
        else if(choice==20) simulator.displaySystemAnalytics();
        else if(choice==21)
        {
            int frames,count;cout<<"\nNumber of frames: ";cin>>frames;cout<<"Reference string length: ";cin>>count;
            if(frames<=0||count<=0){cout<<"\nInvalid benchmark input.\n";continue;}
            vector<int> refs(count);cout<<"Enter reference string: ";for(int&i:refs)cin>>i;
            simulator.runPageReplacementBenchmark(refs,frames);
        }
        else if(choice==22)
        {
            int count;cout<<"\nNumber of allocation requests: ";cin>>count;
            if(count<=0){cout<<"\nInvalid request count.\n";continue;}
            vector<int> reqs(count);cout<<"Enter allocation requests: ";for(int&i:reqs)cin>>i;
            simulator.runAllocationBenchmark(reqs);
        }
        else if(choice==23) simulator.runMemoryAwareScheduling();
        else if(choice==24) simulator.runMLFQ();
        else if(choice==25) simulator.runEventSimulation();
        else if(choice==26) simulator.displaySystemDashboard();
        else if(choice==27) simulator.displayProcessTable();
        else if(choice==28) simulator.displaySystemConfiguration();
        else if(choice==29) simulator.runTests();
        else if(choice==30) simulator.displayEventLog();
        else if(choice==31){simulator.clearEventLog();cout<<"\nEvent log cleared.\n";}
        else if(choice==32)
        {
            int type;cout<<"\n1. Page Replacement Stress Test\n2. TLB Stress Test\nTest: ";cin>>type;
            if(type==1)
            {
                int references,pages,frames;cout<<"References: ";cin>>references;cout<<"Pages: ";cin>>pages;cout<<"Frames: ";cin>>frames;
                simulator.runStressTest(type,references,pages,frames);
            }
            else if(type==2)
            {
                int accesses,pages;cout<<"Accesses: ";cin>>accesses;cout<<"Pages: ";cin>>pages;
                simulator.runStressTest(type,accesses,pages);
            }
            else cout<<"\nInvalid stress-test type.\n";
        }
        else if(choice==33) cout<<(simulator.exportReports()?"\nReports exported to reports/.\n":"\nReport export failed.\n");
        else if(choice==34) cout<<(simulator.runFinalValidation()?"\nFinal validation passed.\n":"\nFinal validation failed.\n");
        else if(choice==35){simulator.getLogger().log("SYSTEM SHUTDOWN");cout<<"\nExiting simulator...\n";}
        else cout<<"\nInvalid choice.\n";
    }while(choice!=35);
    return 0;
}
