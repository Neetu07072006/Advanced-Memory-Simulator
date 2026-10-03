import { useMemo, useState } from "react";

const initialProcesses=[
  {id:1,name:"Process A",arrival:0,burst:7,priority:2,color:"blue"},
  {id:2,name:"Process B",arrival:1,burst:4,priority:1,color:"green"},
  {id:3,name:"Process C",arrival:2,burst:9,priority:3,color:"purple"},
  {id:4,name:"Process D",arrival:3,burst:5,priority:2,color:"orange"}
];

function buildSchedule(processes,algorithm,quantum){
  if(!processes.length)return [];

  const remaining=processes.map(p=>({...p,remaining:p.burst}));
  const result=[];
  let time=0;
  let completed=0;
  const added=new Set();

  const addSegment=(p,start,end)=>{
    if(end<=start)return;
    const previous=result[result.length-1];

    if(previous&&previous.id===p.id&&previous.end===start){
      previous.end=end;
    }else{
      result.push({
        id:p.id,
        name:p.name,
        start,
        end
      });
    }
  };

  if(algorithm==="FCFS"){
    const queue=[...remaining].sort((a,b)=>a.arrival-b.arrival);

    queue.forEach(p=>{
      if(time<p.arrival)time=p.arrival;

      addSegment(p,time,time+p.burst);
      time+=p.burst;
    });

    return result;
  }

  if(algorithm==="SJF"){
    while(completed<remaining.length){
      const available=remaining.filter(
        p=>p.remaining>0&&p.arrival<=time
      );

      if(!available.length){
        time=Math.min(
          ...remaining
            .filter(p=>p.remaining>0)
            .map(p=>p.arrival)
        );
        continue;
      }

      const p=available.sort(
        (a,b)=>a.burst-b.burst||a.arrival-b.arrival
      )[0];

      addSegment(p,time,time+p.remaining);
      time+=p.remaining;
      p.remaining=0;
      completed++;
    }

    return result;
  }

  if(algorithm==="Priority"){
    while(completed<remaining.length){
      const available=remaining.filter(
        p=>p.remaining>0&&p.arrival<=time
      );

      if(!available.length){
        time=Math.min(
          ...remaining
            .filter(p=>p.remaining>0)
            .map(p=>p.arrival)
        );
        continue;
      }

      const p=available.sort(
        (a,b)=>a.priority-b.priority||a.arrival-b.arrival
      )[0];

      addSegment(p,time,time+p.remaining);
      time+=p.remaining;
      p.remaining=0;
      completed++;
    }

    return result;
  }

  if(algorithm==="Round Robin"){
    const queue=[];
    let nextIndex=0;

    while(completed<remaining.length){
      while(
        nextIndex<remaining.length&&
        remaining[nextIndex].arrival<=time
      ){
        queue.push(remaining[nextIndex]);
        nextIndex++;
      }

      if(!queue.length){
        if(nextIndex<remaining.length){
          time=remaining[nextIndex].arrival;
          continue;
        }
      }

      const p=queue.shift();
      const run=Math.min(quantum,p.remaining);

      addSegment(p,time,time+run);
      time+=run;
      p.remaining-=run;

      while(
        nextIndex<remaining.length&&
        remaining[nextIndex].arrival<=time
      ){
        queue.push(remaining[nextIndex]);
        nextIndex++;
      }

      if(p.remaining>0){
        queue.push(p);
      }else{
        completed++;
      }
    }

    return result;
  }

  return result;
}

function calculateMetrics(processes,schedule){
  return processes.map(p=>{
    const segments=schedule.filter(s=>s.id===p.id);

    if(!segments.length){
      return {
        ...p,
        completion:0,
        turnaround:0,
        waiting:0,
        response:0
      };
    }

    const completion=segments[segments.length-1].end;
    const turnaround=completion-p.arrival;
    const waiting=turnaround-p.burst;
    const response=segments[0].start-p.arrival;

    return {
      ...p,
      completion,
      turnaround,
      waiting,
      response
    };
  });
}

function CPUScheduler(){
  const [processes,setProcesses]=useState(initialProcesses);
  const [algorithm,setAlgorithm]=useState("FCFS");
  const [quantum,setQuantum]=useState(2);
  const [name,setName]=useState("");
  const [arrival,setArrival]=useState("");
  const [burst,setBurst]=useState("");
  const [priority,setPriority]=useState("2");
  const [running,setRunning]=useState(false);
  const [selected,setSelected]=useState(null);
  const [message,setMessage]=useState("");

  const schedule=useMemo(
    ()=>buildSchedule(processes,algorithm,Number(quantum)),
    [processes,algorithm,quantum]
  );

  const metrics=useMemo(
    ()=>calculateMetrics(processes,schedule),
    [processes,schedule]
  );

  const averages=useMemo(()=>{
    if(!metrics.length){
      return {
        waiting:0,
        turnaround:0,
        response:0
      };
    }

    return {
      waiting:(
        metrics.reduce((s,p)=>s+p.waiting,0)/metrics.length
      ).toFixed(2),

      turnaround:(
        metrics.reduce((s,p)=>s+p.turnaround,0)/metrics.length
      ).toFixed(2),

      response:(
        metrics.reduce((s,p)=>s+p.response,0)/metrics.length
      ).toFixed(2)
    };
  },[metrics]);

  const totalTime=schedule.length
    ?Math.max(...schedule.map(s=>s.end))
    :0;

  const busyTime=processes.reduce((s,p)=>s+p.burst,0);

  const cpuUtilization=totalTime
    ?Math.min(100,Math.round((busyTime/totalTime)*100))
    :0;

  const addProcess=()=>{
    if(!name.trim()||arrival===""||burst===""){
      setMessage("Enter process name, arrival time and burst time.");
      return;
    }

    const process={
      id:Date.now(),
      name:name.trim(),
      arrival:Number(arrival),
      burst:Number(burst),
      priority:Number(priority),
      color:"blue"
    };

    setProcesses(prev=>[...prev,process]);
    setName("");
    setArrival("");
    setBurst("");
    setPriority("2");
    setMessage(`${process.name} added to the ready queue.`);
  };

  const removeProcess=id=>{
    setProcesses(prev=>prev.filter(p=>p.id!==id));
    setSelected(null);
    setMessage("Process removed.");
  };

  const reset=()=>{
    setProcesses(initialProcesses);
    setAlgorithm("FCFS");
    setQuantum(2);
    setRunning(false);
    setSelected(null);
    setMessage("Scheduler reset.");
  };

  const runSimulation=()=>{
    setRunning(true);
    setMessage(`${algorithm} scheduling simulation executed.`);
  };

  return(
    <section className="page-section">
      <div className="page-heading">
        <div>
          <span className="eyebrow">CPU MANAGEMENT</span>
          <h2>CPU Scheduler</h2>
          <p>
            Compare CPU scheduling algorithms and visualize process execution.
          </p>
        </div>

        <div className="allocator-capacity">
          <span>Scheduler</span>
          <strong>{running?"Running":"Ready"}</strong>
        </div>
      </div>

      {message&&(
        <div className="scheduler-message">
          <span></span>
          {message}
        </div>
      )}

      <div className="scheduler-stats">
        <div className="scheduler-stat">
          <span>Processes</span>
          <strong>{processes.length}</strong>
          <small>Ready queue size</small>
        </div>

        <div className="scheduler-stat">
          <span>Avg Waiting Time</span>
          <strong>{averages.waiting}</strong>
          <small>CPU scheduling delay</small>
        </div>

        <div className="scheduler-stat">
          <span>Avg Turnaround</span>
          <strong>{averages.turnaround}</strong>
          <small>Completion performance</small>
        </div>

        <div className="scheduler-stat">
          <span>Avg Response</span>
          <strong>{averages.response}</strong>
          <small>First CPU response</small>
        </div>

        <div className="scheduler-stat">
          <span>CPU Utilization</span>
          <strong>{cpuUtilization}%</strong>
          <small>{busyTime} / {totalTime||0} time units</small>
        </div>
      </div>

      <div className="scheduler-control-card">
        <div className="card-heading">
          <div>
            <h3>Scheduling Configuration</h3>
            <p>Select the algorithm used to build the execution schedule.</p>
          </div>
        </div>

        <div className="scheduler-controls">
          <label>
            <span>Algorithm</span>
            <select
              value={algorithm}
              onChange={e=>setAlgorithm(e.target.value)}
            >
              <option>FCFS</option>
              <option>SJF</option>
              <option>Priority</option>
              <option>Round Robin</option>
            </select>
          </label>

          <label>
            <span>Time Quantum</span>
            <input
              type="number"
              min="1"
              value={quantum}
              onChange={e=>setQuantum(e.target.value)}
              disabled={algorithm!=="Round Robin"}
            />
          </label>

          <button
            className="primary-button"
            onClick={runSimulation}
          >
            ▶ Run Scheduler
          </button>

          <button
            className="secondary-button"
            onClick={reset}
          >
            Reset
          </button>
        </div>

        <div className="algorithm-description">
          <strong>
            {algorithm==="FCFS"&&"First Come First Serve"}
            {algorithm==="SJF"&&"Shortest Job First"}
            {algorithm==="Priority"&&"Priority Scheduling"}
            {algorithm==="Round Robin"&&"Round Robin"}
          </strong>

          <span>
            {algorithm==="FCFS"&&
              "Processes execute in order of arrival."}
            {algorithm==="SJF"&&
              "The process with the shortest burst time is selected first."}
            {algorithm==="Priority"&&
              "The ready process with the highest priority is selected first."}
            {algorithm==="Round Robin"&&
              "Each process receives CPU time according to the configured quantum."}
          </span>
        </div>
      </div>

      <div className="scheduler-main-grid">
        <div className="ready-queue-card">
          <div className="card-heading">
            <div>
              <h3>Ready Queue</h3>
              <p>Processes waiting for CPU execution.</p>
            </div>
          </div>

          <div className="queue-list">
            {processes
              .slice()
              .sort((a,b)=>a.arrival-b.arrival)
              .map((process,index)=>(
                <div
                  className={`queue-process ${
                    selected===process.id?"queue-selected":""
                  }`}
                  key={process.id}
                  onClick={()=>setSelected(process.id)}
                >
                  <div className="queue-position">
                    {index+1}
                  </div>

                  <div className="queue-process-info">
                    <strong>{process.name}</strong>
                    <span>
                      Arrival {process.arrival} · Burst {process.burst}
                    </span>
                  </div>

                  <div className="queue-priority">
                    P{process.priority}
                  </div>

                  <button
                    className="queue-remove"
                    onClick={e=>{
                      e.stopPropagation();
                      removeProcess(process.id);
                    }}
                  >
                    ×
                  </button>
                </div>
              ))}
          </div>
        </div>

        <div className="add-process-card">
          <div className="card-heading">
            <div>
              <h3>Add Process</h3>
              <p>Insert a new process into the scheduler.</p>
            </div>
          </div>

          <div className="scheduler-form">
            <label>
              <span>Name</span>
              <input
                value={name}
                onChange={e=>setName(e.target.value)}
                placeholder="Process E"
              />
            </label>

            <label>
              <span>Arrival</span>
              <input
                type="number"
                min="0"
                value={arrival}
                onChange={e=>setArrival(e.target.value)}
                placeholder="0"
              />
            </label>

            <label>
              <span>Burst</span>
              <input
                type="number"
                min="1"
                value={burst}
                onChange={e=>setBurst(e.target.value)}
                placeholder="5"
              />
            </label>

            <label>
              <span>Priority</span>
              <select
                value={priority}
                onChange={e=>setPriority(e.target.value)}
              >
                <option value="1">1 - High</option>
                <option value="2">2 - Medium</option>
                <option value="3">3 - Low</option>
                <option value="4">4 - Lowest</option>
              </select>
            </label>

            <button
              className="primary-button"
              onClick={addProcess}
            >
              + Add Process
            </button>
          </div>
        </div>
      </div>

      <div className="gantt-card">
        <div className="card-heading">
          <div>
            <h3>Gantt Chart</h3>
            <p>Visual representation of CPU execution order.</p>
          </div>

          <span className="timeline-total">
            Total: {totalTime} units
          </span>
        </div>

        {schedule.length===0?(
          <div className="empty-schedule">
            Run the scheduler to generate the execution timeline.
          </div>
        ):(
          <div className="gantt-wrapper">
            <div className="gantt-chart">
              {schedule.map((segment,index)=>{
                const width=(
                  (segment.end-segment.start)/totalTime
                )*100;

                return(
                  <div
                    className={`gantt-segment gantt-${index%4}`}
                    style={{width:`${width}%`}}
                    key={`${segment.id}-${index}`}
                    title={`${segment.name}: ${segment.start} - ${segment.end}`}
                  >
                    <strong>{segment.name}</strong>
                    <span>{segment.end-segment.start}</span>
                  </div>
                );
              })}
            </div>

            <div className="gantt-scale">
              {schedule.map((segment,index)=>(
                <span
                  key={`${segment.id}-scale-${index}`}
                  style={{
                    left:`${(segment.start/totalTime)*100}%`
                  }}
                >
                  {segment.start}
                </span>
              ))}
              <span className="gantt-end">{totalTime}</span>
            </div>
          </div>
        )}
      </div>

      <div className="metrics-card">
        <div className="card-heading">
          <div>
            <h3>Scheduling Metrics</h3>
            <p>Calculated performance for every process.</p>
          </div>
        </div>

        <div className="metrics-table">
          <div className="metrics-head">
            <span>Process</span>
            <span>Arrival</span>
            <span>Burst</span>
            <span>Priority</span>
            <span>Completion</span>
            <span>Turnaround</span>
            <span>Waiting</span>
            <span>Response</span>
          </div>

          {metrics.map(process=>(
            <div className="metrics-row" key={process.id}>
              <strong>{process.name}</strong>
              <span>{process.arrival}</span>
              <span>{process.burst}</span>
              <span>P{process.priority}</span>
              <span>{process.completion}</span>
              <span>{process.turnaround}</span>
              <span>{process.waiting}</span>
              <span>{process.response}</span>
            </div>
          ))}
        </div>
      </div>

      <div className="scheduler-bottom-grid">
        <div className="scheduler-info-card">
          <h3>Scheduling Algorithms</h3>

          <div className="algorithm-list">
            <div>
              <strong>FCFS</strong>
              <span>Non-preemptive · Arrival order</span>
            </div>

            <div>
              <strong>SJF</strong>
              <span>Non-preemptive · Shortest burst</span>
            </div>

            <div>
              <strong>Priority</strong>
              <span>Non-preemptive · Highest priority</span>
            </div>

            <div>
              <strong>Round Robin</strong>
              <span>Preemptive · Time quantum</span>
            </div>
          </div>
        </div>

        <div className="scheduler-info-card">
          <h3>Execution Pipeline</h3>

          <div className="scheduler-flow">
            <div>
              <b>1</b>
              <span>Ready Queue</span>
            </div>
            <i>→</i>
            <div>
              <b>2</b>
              <span>Scheduler</span>
            </div>
            <i>→</i>
            <div>
              <b>3</b>
              <span>CPU</span>
            </div>
            <i>→</i>
            <div>
              <b>4</b>
              <span>Completed</span>
            </div>
          </div>
        </div>
      </div>
    </section>
  );
}

export default CPUScheduler;