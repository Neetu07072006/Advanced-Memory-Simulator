import {useEffect,useState} from "react";
import StatCard from "../components/StatCard";
import MemoryMap from "../components/MemoryMap";
import ActivityPanel from "../components/ActivityPanel";

function Overview(){
  const [running,setRunning]=useState(false);
  const [memory,setMemory]=useState(62);
  const [processes,setProcesses]=useState(8);
  const [faults,setFaults]=useState(24);
  const [tlb,setTlb]=useState(86);
  const [cycle,setCycle]=useState(0);

  const [chartData,setChartData]=useState([
    35,42,38,55,51,68,62,72,62
  ]);

  useEffect(()=>{
    if(!running) return;

    const timer=setInterval(()=>{
      setCycle(prev=>prev+1);

      setMemory(prev=>{
        const change=Math.floor(Math.random()*9)-4;
        return Math.min(92,Math.max(35,prev+change));
      });

      setProcesses(prev=>{
        const change=Math.floor(Math.random()*3)-1;
        return Math.min(14,Math.max(4,prev+change));
      });

      setFaults(prev=>{
        const change=Math.floor(Math.random()*5);
        return prev+change;
      });

      setTlb(prev=>{
        const change=Math.floor(Math.random()*7)-3;
        return Math.min(98,Math.max(70,prev+change));
      });

      setChartData(prev=>{
        const next=[...prev.slice(1)];
        const value=Math.floor(Math.random()*35)+45;
        next.push(value);
        return next;
      });
    },1500);

    return()=>clearInterval(timer);
  },[running]);

  const handleSimulation=()=>{
    setRunning(prev=>!prev);
  };

  return(
    <div className="page">

      <div className="page-heading">
        <div>
          <span className="eyebrow">SIMULATION CENTER</span>

          <h1>System Overview</h1>

          <p>
            Monitor memory allocation, processes, paging and system activity.
          </p>
        </div>

        <button
          className={`primary-button ${running?"stop-button":""}`}
          onClick={handleSimulation}
        >
          {running?"■ Stop Simulation":"▶ Start Simulation"}
        </button>
      </div>

      <div className="simulation-status">
        <span className={running?"running-dot":"paused-dot"}></span>

        <strong>
          {running?"Simulation Running":"Simulation Ready"}
        </strong>

        <span>
          {running
            ?`System is currently processing simulation events • Cycle ${cycle}`
            :"Start the simulation to begin system activity."}
        </span>
      </div>

      <div className="stats-grid">

        <StatCard
          title="Memory Usage"
          value={`${memory}%`}
          subtitle={`${memory*10} KB of 1000 KB`}
          icon="▥"
          type="blue"
          trend={{
            icon:memory>=62?"↗":"↘",
            value:`${Math.abs(memory-62)}%`,
            type:memory>=62?"positive":"negative"
          }}
        />

        <StatCard
          title="Active Processes"
          value={processes}
          subtitle={`${Math.max(2,Math.floor(processes*.4))} currently running`}
          icon="◉"
          type="purple"
          trend={{
            icon:processes>=8?"↗":"↘",
            value:`${Math.abs(processes-8)}`,
            type:processes>=8?"positive":"negative"
          }}
        />

        <StatCard
          title="Page Faults"
          value={faults}
          subtitle={`${Math.max(1,Math.floor(faults/8))} in current cycle`}
          icon="◫"
          type="orange"
          trend={{
            icon:"↗",
            value:"active",
            type:"negative"
          }}
        />

        <StatCard
          title="TLB Hit Rate"
          value={`${tlb}%`}
          subtitle={`${Math.floor(tlb*.5)} hits / 50 requests`}
          icon="✓"
          type="green"
          trend={{
            icon:tlb>=86?"↗":"↘",
            value:`${Math.abs(tlb-86)}%`,
            type:tlb>=86?"positive":"negative"
          }}
        />

      </div>

      <div className="dashboard-grid">
        <MemoryMap/>
        <ActivityPanel running={running}/>
      </div>

      <div className="card utilization-card">

        <div className="section-header">
          <div>
            <h2>Memory Utilization</h2>
            <p>Real-time simulated memory usage</p>
          </div>

          <strong className="chart-value">
            {memory}%
          </strong>
        </div>

        <div className="chart-area">

          <div className="chart-grid-line line-one"></div>
          <div className="chart-grid-line line-two"></div>
          <div className="chart-grid-line line-three"></div>

          <div className="chart-bars">
            {chartData.map((value,index)=>(
              <span
                key={index}
                style={{height:`${value}%`}}
              ></span>
            ))}
          </div>

        </div>

        <div className="chart-labels">
          <span>0s</span>
          <span>2s</span>
          <span>4s</span>
          <span>6s</span>
          <span>8s</span>
          <span>10s</span>
        </div>

      </div>

    </div>
  );
}

export default Overview;