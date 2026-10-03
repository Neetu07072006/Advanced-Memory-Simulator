import { useMemo, useState } from "react";

const initialPages=[
  {id:1,page:0,process:"Process A",frame:0,status:"RAM"},
  {id:2,page:1,process:"Process A",frame:1,status:"RAM"},
  {id:3,page:2,process:"Process B",frame:2,status:"RAM"},
  {id:4,page:3,process:"Process B",frame:3,status:"RAM"},
  {id:5,page:4,process:"Process C",frame:4,status:"RAM"},
  {id:6,page:5,process:"Process C",frame:null,status:"SWAP",slot:0},
  {id:7,page:6,process:"Process D",frame:null,status:"SWAP",slot:1},
  {id:8,page:7,process:"Process D",frame:null,status:"SWAP",slot:2}
];

const FRAME_COUNT=6;
const SWAP_COUNT=8;

function SwapManagement(){
  const [pages,setPages]=useState(initialPages);
  const [selected,setSelected]=useState(null);
  const [logs,setLogs]=useState([]);
  const [nextFrame,setNextFrame]=useState(5);
  const [nextSlot,setNextSlot]=useState(3);
  const [message,setMessage]=useState("");

  const stats=useMemo(()=>{
    const ram=pages.filter(p=>p.status==="RAM").length;
    const swap=pages.filter(p=>p.status==="SWAP").length;

    return {
      ram,
      swap,
      freeFrames:FRAME_COUNT-ram,
      usedSlots:swap,
      freeSlots:SWAP_COUNT-swap,
      utilization:Math.round((swap/SWAP_COUNT)*100)
    };
  },[pages]);

  const addLog=(type,page)=>{
    setLogs(prev=>[
      {
        id:Date.now(),
        type,
        page:page.page,
        process:page.process,
        time:new Date().toLocaleTimeString()
      },
      ...prev
    ].slice(0,8));
  };

  const swapOut=pageId=>{
    const page=pages.find(p=>p.id===pageId);

    if(!page||page.status==="SWAP")return;

    if(stats.freeSlots<=0){
      setMessage("Swap space is full. Cannot swap out this page.");
      return;
    }

    const slot=nextSlot%SWAP_COUNT;

    setPages(prev=>prev.map(p=>{
      if(p.id!==pageId)return p;

      return {
        ...p,
        status:"SWAP",
        slot,
        frame:null
      };
    }));

    setNextSlot(prev=>(prev+1)%SWAP_COUNT);
    setSelected(null);
    setMessage(`Page ${page.page} moved from RAM to swap slot ${slot}.`);
    addLog("out",page);
  };

  const swapIn=pageId=>{
    const page=pages.find(p=>p.id===pageId);

    if(!page||page.status==="RAM")return;

    if(stats.freeFrames<=0){
      setMessage("Physical memory is full. Swap out a RAM page first.");
      return;
    }

    const frame=nextFrame%FRAME_COUNT;

    setPages(prev=>prev.map(p=>{
      if(p.id!==pageId)return p;

      return {
        ...p,
        status:"RAM",
        frame,
        slot:null
      };
    }));

    setNextFrame(prev=>(prev+1)%FRAME_COUNT);
    setSelected(null);
    setMessage(`Page ${page.page} loaded from swap into frame ${frame}.`);
    addLog("in",page);
  };

  const reset=()=>{
    setPages(initialPages);
    setSelected(null);
    setLogs([]);
    setNextFrame(5);
    setNextSlot(3);
    setMessage("Swap manager reset.");
  };

  const selectedPage=pages.find(p=>p.id===selected);

  return(
    <section className="page-section">
      <div className="page-heading">
        <div>
          <span className="eyebrow">VIRTUAL MEMORY</span>
          <h2>Swap Management</h2>
          <p>Simulate movement of pages between physical memory and swap space.</p>
        </div>

        <div className="allocator-capacity">
          <span>Swap Manager</span>
          <strong>Active</strong>
        </div>
      </div>

      {message&&(
        <div className="swap-message">
          <span></span>
          {message}
        </div>
      )}

      <div className="swap-stats">
        <div className="swap-stat">
          <span>RAM Pages</span>
          <strong>{stats.ram}</strong>
          <small>Pages currently resident</small>
        </div>

        <div className="swap-stat">
          <span>Free Frames</span>
          <strong>{stats.freeFrames}</strong>
          <small>Available physical frames</small>
        </div>

        <div className="swap-stat">
          <span>Swap Used</span>
          <strong>{stats.usedSlots}</strong>
          <small>Occupied swap slots</small>
        </div>

        <div className="swap-stat">
          <span>Free Swap</span>
          <strong>{stats.freeSlots}</strong>
          <small>Available swap slots</small>
        </div>

        <div className="swap-stat">
          <span>Swap Utilization</span>
          <strong>{stats.utilization}%</strong>
          <small>{SWAP_COUNT} total slots</small>
        </div>
      </div>

      <div className="swap-memory-grid">
        <div className="swap-memory-card">
          <div className="card-heading">
            <div>
              <h3>Physical Memory</h3>
              <p>Pages currently loaded into RAM frames.</p>
            </div>
            <span className="memory-capacity-badge">
              {FRAME_COUNT} Frames
            </span>
          </div>

          <div className="frame-grid">
            {Array.from({length:FRAME_COUNT},(_,index)=>{
              const page=pages.find(
                p=>p.status==="RAM"&&p.frame===index
              );

              return(
                <div
                  className={`memory-frame ${page?"occupied":"empty"} ${
                    selected===page?.id?"frame-selected":""
                  }`}
                  key={index}
                  onClick={()=>page&&setSelected(page.id)}
                >
                  <div className="frame-number">
                    Frame {index}
                  </div>

                  {page?(
                    <>
                      <strong>Page {page.page}</strong>
                      <span>{page.process}</span>
                      <small>Resident</small>
                    </>
                  ):(
                    <>
                      <strong>Free</strong>
                      <span>No page mapped</span>
                      <small>Available</small>
                    </>
                  )}
                </div>
              );
            })}
          </div>
        </div>

        <div className="swap-space-card">
          <div className="card-heading">
            <div>
              <h3>Swap Space</h3>
              <p>Pages temporarily stored outside physical memory.</p>
            </div>
            <span className="swap-capacity-badge">
              {SWAP_COUNT} Slots
            </span>
          </div>

          <div className="swap-slot-grid">
            {Array.from({length:SWAP_COUNT},(_,index)=>{
              const page=pages.find(
                p=>p.status==="SWAP"&&p.slot===index
              );

              return(
                <div
                  className={`swap-slot ${page?"occupied":"empty"} ${
                    selected===page?.id?"slot-selected":""
                  }`}
                  key={index}
                  onClick={()=>page&&setSelected(page.id)}
                >
                  <div>Slot {index}</div>

                  {page?(
                    <>
                      <strong>Page {page.page}</strong>
                      <span>{page.process}</span>
                    </>
                  ):(
                    <>
                      <strong>Free</strong>
                      <span>Available</span>
                    </>
                  )}
                </div>
              );
            })}
          </div>

          <div className="swap-utilization">
            <div>
              <span>Swap utilization</span>
              <strong>{stats.utilization}%</strong>
            </div>
            <div className="swap-progress">
              <div style={{width:`${stats.utilization}%`}}></div>
            </div>
          </div>
        </div>
      </div>

      <div className="swap-main-grid">
        <div className="swap-page-table">
          <div className="card-heading">
            <div>
              <h3>Page Residency Table</h3>
              <p>Current location of every simulated page.</p>
            </div>
          </div>

          <div className="swap-table">
            <div className="swap-table-head">
              <span>Page</span>
              <span>Process</span>
              <span>Location</span>
              <span>Frame</span>
              <span>Swap Slot</span>
              <span>Status</span>
            </div>

            {pages.map(page=>(
              <div
                className={`swap-table-row ${
                  selected===page.id?"swap-row-selected":""
                }`}
                key={page.id}
                onClick={()=>setSelected(page.id)}
              >
                <span className="page-number">Page {page.page}</span>
                <span>{page.process}</span>

                <span>
                  <b className={`location-badge ${page.status.toLowerCase()}`}>
                    {page.status==="RAM"?"Physical Memory":"Swap Space"}
                  </b>
                </span>

                <span>
                  {page.frame!==null?`Frame ${page.frame}`:"—"}
                </span>

                <span>
                  {page.slot!==undefined&&page.slot!==null
                    ?`Slot ${page.slot}`
                    :"—"}
                </span>

                <span>
                  <b className={`residency-badge ${page.status.toLowerCase()}`}>
                    {page.status==="RAM"?"Resident":"Swapped"}
                  </b>
                </span>
              </div>
            ))}
          </div>
        </div>

        <aside className="swap-control-panel">
          <div className="card-heading">
            <div>
              <h3>Page Controls</h3>
              <p>Select a page to perform swap operations.</p>
            </div>
          </div>

          {selectedPage?(
            <>
              <div className="selected-page-card">
                <span>Selected Page</span>
                <strong>Page {selectedPage.page}</strong>
                <small>{selectedPage.process}</small>
              </div>

              <div className="swap-detail-grid">
                <div>
                  <span>Location</span>
                  <strong>{selectedPage.status==="RAM"?"RAM":"Swap"}</strong>
                </div>

                <div>
                  <span>Frame</span>
                  <strong>
                    {selectedPage.frame!==null
                      ?selectedPage.frame
                      :"—"}
                  </strong>
                </div>

                <div>
                  <span>Swap Slot</span>
                  <strong>
                    {selectedPage.slot!==undefined&&selectedPage.slot!==null
                      ?selectedPage.slot
                      :"—"}
                  </strong>
                </div>

                <div>
                  <span>Process</span>
                  <strong>{selectedPage.process}</strong>
                </div>
              </div>

              <div className="swap-actions">
                <button
                  className="primary-button"
                  onClick={()=>swapOut(selectedPage.id)}
                  disabled={selectedPage.status==="SWAP"}
                >
                  ↓ Swap Out
                </button>

                <button
                  className="secondary-button"
                  onClick={()=>swapIn(selectedPage.id)}
                  disabled={selectedPage.status==="RAM"}
                >
                  ↑ Swap In
                </button>
              </div>
            </>
          ):(
            <div className="swap-empty">
              <div>⇄</div>
              <strong>No Page Selected</strong>
              <span>
                Select a page from RAM or swap space to manage its location.
              </span>
            </div>
          )}

          <button className="secondary-button swap-reset" onClick={reset}>
            Reset Swap Manager
          </button>
        </aside>
      </div>

      <div className="swap-bottom-grid">
        <div className="swap-log-card">
          <div className="card-heading">
            <div>
              <h3>Swap Activity</h3>
              <p>Recent page movement between RAM and swap.</p>
            </div>
          </div>

          {logs.length===0?(
            <div className="empty-swap-log">
              No swap operations performed yet.
            </div>
          ):(
            <div className="swap-log">
              {logs.map(log=>(
                <div className="swap-log-row" key={log.id}>
                  <div className={`swap-log-icon ${log.type}`}>
                    {log.type==="in"?"↑":"↓"}
                  </div>

                  <div>
                    <strong>
                      Page {log.page} · {log.process}
                    </strong>
                    <small>
                      {log.type==="in"
                        ?"Loaded from swap into RAM"
                        :"Moved from RAM to swap"}
                    </small>
                  </div>

                  <span>{log.time}</span>
                </div>
              ))}
            </div>
          )}
        </div>

        <div className="swap-info-card">
          <h3>Swap Workflow</h3>

          <div className="swap-flow">
            <div>
              <b>1</b>
              <span>Page selected</span>
            </div>

            <i>→</i>

            <div>
              <b>2</b>
              <span>Check available space</span>
            </div>

            <i>→</i>

            <div>
              <b>3</b>
              <span>Move page</span>
            </div>

            <i>→</i>

            <div>
              <b>4</b>
              <span>Update mapping</span>
            </div>
          </div>

          <div className="swap-note">
            <strong>OS Concept</strong>
            <span>
              Swapping temporarily moves a page between physical memory and
              secondary storage when memory resources need to be managed.
            </span>
          </div>
        </div>
      </div>
    </section>
  );
}

export default SwapManagement;