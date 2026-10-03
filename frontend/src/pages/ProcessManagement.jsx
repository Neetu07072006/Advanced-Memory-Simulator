import { useMemo, useState } from "react";

const initialProcesses = [
  { id:1,name:"Process A",memory:120,priority:1,burst:8,strategy:"First Fit",state:"Running",progress:72 },
  { id:2,name:"Process B",memory:180,priority:2,burst:12,strategy:"Best Fit",state:"Ready",progress:38 },
  { id:3,name:"Process C",memory:220,priority:3,burst:15,strategy:"Worst Fit",state:"Ready",progress:21 },
  { id:4,name:"Process D",memory:100,priority:1,burst:6,strategy:"Next Fit",state:"Waiting",progress:0 }
];

function ProcessManagement() {
  const [processes,setProcesses] = useState(initialProcesses);
  const [name,setName] = useState("");
  const [memory,setMemory] = useState("");
  const [priority,setPriority] = useState("1");
  const [burst,setBurst] = useState("");
  const [strategy,setStrategy] = useState("First Fit");
  const [selected,setSelected] = useState(null);
  const [message,setMessage] = useState("");

  const stats = useMemo(() => ({
    total:processes.length,
    running:processes.filter(p=>p.state==="Running").length,
    ready:processes.filter(p=>p.state==="Ready").length,
    waiting:processes.filter(p=>p.state==="Waiting").length,
    memory:processes.reduce((sum,p)=>sum+p.memory,0)
  }),[processes]);

  const addProcess = () => {
    if(!name.trim() || !memory || !burst){
      setMessage("Enter process name, memory and burst time.");
      return;
    }

    const newProcess = {
      id:Date.now(),
      name:name.trim(),
      memory:Number(memory),
      priority:Number(priority),
      burst:Number(burst),
      strategy,
      state:"Ready",
      progress:0
    };

    setProcesses(prev=>[...prev,newProcess]);
    setName("");
    setMemory("");
    setBurst("");
    setMessage(`${newProcess.name} created successfully.`);
  };

  const changeState = (id,state) => {
    setProcesses(prev=>prev.map(p=>{
      if(p.id!==id) return p;

      let progress=p.progress;

      if(state==="Running" && progress===0) progress=5;
      if(state==="Terminated") progress=100;

      return {...p,state,progress};
    }));

    const process=processes.find(p=>p.id===id);
    if(process) setMessage(`${process.name} changed to ${state}.`);
  };

  const terminateProcess = id => {
    const process=processes.find(p=>p.id===id);
    setProcesses(prev=>prev.filter(p=>p.id!==id));
    if(process) setMessage(`${process.name} terminated and removed.`);
    setSelected(null);
  };

  const runProcess = id => {
    setProcesses(prev=>prev.map(p=>{
      if(p.id!==id) return p;

      const progress=Math.min(100,p.progress+10);

      return {
        ...p,
        state:progress>=100?"Terminated":"Running",
        progress
      };
    }));

    const process=processes.find(p=>p.id===id);
    if(process) setMessage(`${process.name} executed for one simulation cycle.`);
  };

  const reset = () => {
    setProcesses(initialProcesses);
    setSelected(null);
    setMessage("Process manager reset.");
  };

  const selectedProcess=processes.find(p=>p.id===selected);

  return (
    <section className="page-section">
      <div className="page-heading">
        <div>
          <span className="eyebrow">PROCESS CONTROL</span>
          <h2>Process Management</h2>
          <p>Create, schedule and monitor processes inside the memory simulator.</p>
        </div>
        <div className="allocator-capacity">
          <span>Process Manager</span>
          <strong>{stats.total} Processes</strong>
        </div>
      </div>

      {message && (
        <div className="process-message">
          <span></span>
          {message}
        </div>
      )}

      <div className="process-stats">
        <div className="process-stat">
          <span>Total Processes</span>
          <strong>{stats.total}</strong>
          <small>Active process table</small>
        </div>
        <div className="process-stat">
          <span>Running</span>
          <strong>{stats.running}</strong>
          <small>Currently executing</small>
        </div>
        <div className="process-stat">
          <span>Ready</span>
          <strong>{stats.ready}</strong>
          <small>Waiting for CPU</small>
        </div>
        <div className="process-stat">
          <span>Waiting</span>
          <strong>{stats.waiting}</strong>
          <small>Blocked processes</small>
        </div>
        <div className="process-stat">
          <span>Allocated Memory</span>
          <strong>{stats.memory} KB</strong>
          <small>Process memory demand</small>
        </div>
      </div>

      <div className="process-create-card">
        <div className="card-heading">
          <div>
            <h3>Create Process</h3>
            <p>Add a new process to the simulation.</p>
          </div>
          <span className="card-icon">+</span>
        </div>

        <div className="process-form">
          <label>
            <span>Process Name</span>
            <input
              value={name}
              onChange={e=>setName(e.target.value)}
              placeholder="e.g. Process E"
            />
          </label>

          <label>
            <span>Memory Required</span>
            <input
              type="number"
              min="1"
              value={memory}
              onChange={e=>setMemory(e.target.value)}
              placeholder="KB"
            />
          </label>

          <label>
            <span>Priority</span>
            <select value={priority} onChange={e=>setPriority(e.target.value)}>
              <option value="1">1 - High</option>
              <option value="2">2 - Medium</option>
              <option value="3">3 - Low</option>
              <option value="4">4 - Lowest</option>
            </select>
          </label>

          <label>
            <span>Burst Time</span>
            <input
              type="number"
              min="1"
              value={burst}
              onChange={e=>setBurst(e.target.value)}
              placeholder="Cycles"
            />
          </label>

          <label>
            <span>Allocation Strategy</span>
            <select value={strategy} onChange={e=>setStrategy(e.target.value)}>
              <option>First Fit</option>
              <option>Best Fit</option>
              <option>Worst Fit</option>
              <option>Next Fit</option>
            </select>
          </label>

          <button className="primary-button process-add-button" onClick={addProcess}>
            + Create Process
          </button>
        </div>
      </div>

      <div className="process-main-grid">
        <div className="process-table-card">
          <div className="card-heading">
            <div>
              <h3>Process Control Block</h3>
              <p>Live process information and execution state.</p>
            </div>
            <button className="secondary-button" onClick={reset}>Reset</button>
          </div>

          <div className="process-table">
            <div className="process-table-head">
              <span>PID</span>
              <span>Process</span>
              <span>Memory</span>
              <span>Priority</span>
              <span>Burst</span>
              <span>Strategy</span>
              <span>State</span>
              <span>Progress</span>
            </div>

            {processes.map(process=>(
              <div
                className={`process-table-row ${selected===process.id?"process-selected":""}`}
                key={process.id}
                onClick={()=>setSelected(process.id)}
              >
                <span className="pid-cell">P{process.id}</span>
                <span className="process-name-cell">{process.name}</span>
                <span>{process.memory} KB</span>
                <span>
                  <b className={`priority priority-${process.priority}`}>
                    P{process.priority}
                  </b>
                </span>
                <span>{process.burst}</span>
                <span>{process.strategy}</span>
                <span>
                  <b className={`state-badge state-${process.state.toLowerCase()}`}>
                    {process.state}
                  </b>
                </span>
                <span>
                  <div className="mini-progress">
                    <div style={{width:`${process.progress}%`}}></div>
                  </div>
                  <small>{process.progress}%</small>
                </span>
              </div>
            ))}
          </div>
        </div>

        <aside className="process-control-panel">
          <div className="card-heading">
            <div>
              <h3>Process Controls</h3>
              <p>Select a process to control it.</p>
            </div>
          </div>

          {selectedProcess ? (
            <>
              <div className="selected-process-card">
                <span>Selected Process</span>
                <strong>{selectedProcess.name}</strong>
                <small>PID: P{selectedProcess.id}</small>
              </div>

              <div className="process-detail-grid">
                <div>
                  <span>Memory</span>
                  <strong>{selectedProcess.memory} KB</strong>
                </div>
                <div>
                  <span>Priority</span>
                  <strong>P{selectedProcess.priority}</strong>
                </div>
                <div>
                  <span>Burst</span>
                  <strong>{selectedProcess.burst}</strong>
                </div>
                <div>
                  <span>State</span>
                  <strong>{selectedProcess.state}</strong>
                </div>
              </div>

              <div className="process-actions">
                <button
                  className="primary-button"
                  onClick={()=>runProcess(selectedProcess.id)}
                  disabled={selectedProcess.state==="Terminated"}
                >
                  Run Cycle
                </button>

                <button
                  className="secondary-button"
                  onClick={()=>changeState(selectedProcess.id,"Ready")}
                >
                  Ready
                </button>

                <button
                  className="secondary-button"
                  onClick={()=>changeState(selectedProcess.id,"Waiting")}
                >
                  Wait
                </button>

                <button
                  className="secondary-button"
                  onClick={()=>changeState(selectedProcess.id,"Running")}
                >
                  Run
                </button>

                <button
                  className="danger-button"
                  onClick={()=>terminateProcess(selectedProcess.id)}
                >
                  Terminate
                </button>
              </div>
            </>
          ) : (
            <div className="process-empty">
              <div>◉</div>
              <strong>No Process Selected</strong>
              <span>Click any process in the table to view its controls.</span>
            </div>
          )}
        </aside>
      </div>

      <div className="process-bottom-grid">
        <div className="process-info-card">
          <h3>Process Lifecycle</h3>

          <div className="lifecycle">
            <div className="lifecycle-step active">
              <strong>NEW</strong>
              <span>Process created</span>
            </div>
            <div className="lifecycle-arrow">→</div>
            <div className="lifecycle-step">
              <strong>READY</strong>
              <span>Waiting for CPU</span>
            </div>
            <div className="lifecycle-arrow">→</div>
            <div className="lifecycle-step">
              <strong>RUNNING</strong>
              <span>CPU execution</span>
            </div>
            <div className="lifecycle-arrow">→</div>
            <div className="lifecycle-step">
              <strong>WAITING</strong>
              <span>Blocked</span>
            </div>
            <div className="lifecycle-arrow">→</div>
            <div className="lifecycle-step">
              <strong>TERMINATED</strong>
              <span>Execution complete</span>
            </div>
          </div>
        </div>

        <div className="process-info-card">
          <h3>Allocation Strategy</h3>
          <div className="strategy-info">
            <div><strong>First Fit</strong><span>First suitable free block</span></div>
            <div><strong>Best Fit</strong><span>Smallest suitable free block</span></div>
            <div><strong>Worst Fit</strong><span>Largest available free block</span></div>
            <div><strong>Next Fit</strong><span>Continue search from previous position</span></div>
          </div>
        </div>
      </div>
    </section>
  );
}

export default ProcessManagement;