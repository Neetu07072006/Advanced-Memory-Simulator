import {useMemo,useState} from "react";

function simulateFIFO(pages,framesCount){
  const frames=[];
  const steps=[];
  let pointer=0;
  let faults=0;
  let hits=0;

  pages.forEach(page=>{
    const hit=frames.includes(page);

    if(hit){
      hits++;
    }else{
      faults++;

      if(frames.length<framesCount){
        frames.push(page);
      }else{
        frames[pointer]=page;
        pointer=(pointer+1)%framesCount;
      }
    }

    steps.push({
      page,
      frames:[...frames],
      hit,
      fault:!hit
    });
  });

  return {steps,faults,hits};
}

function simulateLRU(pages,framesCount){
  const frames=[];
  const steps=[];
  let faults=0;
  let hits=0;

  pages.forEach(page=>{
    const index=frames.indexOf(page);
    const hit=index!==-1;

    if(hit){
      hits++;
      frames.splice(index,1);
      frames.push(page);
    }else{
      faults++;

      if(frames.length>=framesCount){
        frames.shift();
      }

      frames.push(page);
    }

    steps.push({
      page,
      frames:[...frames],
      hit,
      fault:!hit
    });
  });

  return {steps,faults,hits};
}

function simulateOptimal(pages,framesCount){
  const frames=[];
  const steps=[];
  let faults=0;
  let hits=0;

  pages.forEach((page,currentIndex)=>{
    const hit=frames.includes(page);

    if(hit){
      hits++;
    }else{
      faults++;

      if(frames.length<framesCount){
        frames.push(page);
      }else{
        let replaceIndex=0;
        let farthest=-1;

        frames.forEach((frame,index)=>{
          const nextUse=pages.indexOf(frame,currentIndex+1);

          if(nextUse===-1){
            replaceIndex=index;
            farthest=Infinity;
          }else if(nextUse>farthest){
            farthest=nextUse;
            replaceIndex=index;
          }
        });

        frames[replaceIndex]=page;
      }
    }

    steps.push({
      page,
      frames:[...frames],
      hit,
      fault:!hit
    });
  });

  return {steps,faults,hits};
}

function PageReplacement(){
  const [algorithm,setAlgorithm]=useState("FIFO");
  const [referenceInput,setReferenceInput]=useState(
    "7 0 1 2 0 3 0 4 2 3 0 3 2"
  );
  const [frameCount,setFrameCount]=useState(3);
  const [step,setStep]=useState(0);

  const pages=useMemo(()=>{
    return referenceInput
      .trim()
      .split(/[\s,]+/)
      .filter(Boolean)
      .map(Number)
      .filter(Number.isFinite);
  },[referenceInput]);

  const result=useMemo(()=>{
    if(!pages.length)return {steps:[],faults:0,hits:0};

    if(algorithm==="LRU"){
      return simulateLRU(pages,frameCount);
    }

    if(algorithm==="Optimal"){
      return simulateOptimal(pages,frameCount);
    }

    return simulateFIFO(pages,frameCount);
  },[pages,frameCount,algorithm]);

  const visibleSteps=result.steps.slice(0,step);

  const hitRatio=pages.length
    ?((result.hits/pages.length)*100).toFixed(1)
    :0;

  const faultRatio=pages.length
    ?((result.faults/pages.length)*100).toFixed(1)
    :0;

  const runSimulation=()=>{
    setStep(result.steps.length);
  };

  const nextStep=()=>{
    setStep(prev=>Math.min(prev+1,result.steps.length));
  };

  const reset=()=>{
    setStep(0);
  };

  const changeAlgorithm=value=>{
    setAlgorithm(value);
    setStep(0);
  };

  return(
    <div className="page">

      <div className="page-heading">

        <div>
          <span className="eyebrow">MEMORY MANAGEMENT</span>

          <h1>Page Replacement</h1>

          <p>
            Simulate page replacement algorithms and analyze frame behavior.
          </p>
        </div>

        <button
          className="primary-button"
          onClick={runSimulation}
        >
          ▶ Run Simulation
        </button>

      </div>

      <div className="replacement-controls card">

        <div className="replacement-control">

          <label>Algorithm</label>

          <div className="algorithm-buttons">

            {["FIFO","LRU","Optimal"].map(item=>(
              <button
                key={item}
                className={algorithm===item?"selected":""}
                onClick={()=>changeAlgorithm(item)}
              >
                {item}
              </button>
            ))}

          </div>

        </div>

        <div className="replacement-control">

          <label>Number of Frames</label>

          <input
            type="number"
            min="1"
            max="8"
            value={frameCount}
            onChange={e=>{
              setFrameCount(
                Math.min(8,Math.max(1,Number(e.target.value)))
              );
              setStep(0);
            }}
          />

        </div>

        <div className="replacement-control reference-control">

          <label>Reference String</label>

          <input
            value={referenceInput}
            onChange={e=>{
              setReferenceInput(e.target.value);
              setStep(0);
            }}
            placeholder="Example: 7 0 1 2 0 3"
          />

        </div>

        <button
          className="secondary-button"
          onClick={reset}
        >
          ↻ Reset
        </button>

      </div>

      <div className="replacement-summary">

        <div className="replacement-stat">
          <span>Reference Pages</span>
          <strong>{pages.length}</strong>
        </div>

        <div className="replacement-stat">
          <span>Page Faults</span>
          <strong className="fault-number">
            {result.faults}
          </strong>
        </div>

        <div className="replacement-stat">
          <span>Page Hits</span>
          <strong className="hit-number">
            {result.hits}
          </strong>
        </div>

        <div className="replacement-stat">
          <span>Hit Ratio</span>
          <strong>{hitRatio}%</strong>
        </div>

        <div className="replacement-stat">
          <span>Fault Ratio</span>
          <strong>{faultRatio}%</strong>
        </div>

      </div>

      <div className="replacement-main-grid">

        <div className="card simulation-table-card">

          <div className="section-header">

            <div>
              <h2>Frame Simulation</h2>
              <p>
                Step {step} of {result.steps.length}
              </p>
            </div>

            <span className="algorithm-badge">
              {algorithm}
            </span>

          </div>

          <div className="frame-table" style={{ "--frames": frameCount }}>

            <div className="frame-table-head">
              <span>Step</span>
              <span>Page</span>

              {Array.from(
                {length:frameCount},
                (_,index)=>(
                  <span key={index}>
                    Frame {index+1}
                  </span>
                )
              )}

              <span>Status</span>
            </div>

            {visibleSteps.length===0 ? (

              <div className="empty-simulation">
                <div className="empty-icon">▣</div>
                <strong>Simulation Ready</strong>
                <span>
                  Run the simulation to visualize page replacement.
                </span>
              </div>

            ):(
              visibleSteps.map((item,index)=>(
                <div
                  className={`frame-table-row ${
                    item.hit?"row-hit":"row-fault"
                  }`}
                  key={index}
                >

                  <span className="step-number">
                    {index+1}
                  </span>

                  <strong className="page-number">
                    {item.page}
                  </strong>

                  {Array.from(
                    {length:frameCount},
                    (_,frameIndex)=>(
                      <span
                        className={
                          item.frames[frameIndex]===undefined
                            ?"empty-frame"
                            :"frame-value"
                        }
                        key={frameIndex}
                      >
                        {item.frames[frameIndex] ?? "—"}
                      </span>
                    )
                  )}

                  <span
                    className={
                      item.hit
                        ?"replacement-hit"
                        :"replacement-fault"
                    }
                  >
                    {item.hit?"HIT":"FAULT"}
                  </span>

                </div>
              ))
            )}

          </div>

          {step<result.steps.length && step>0 && (
            <button
              className="next-step-button"
              onClick={nextStep}
            >
              Next Step →
            </button>
          )}

        </div>

        <div className="replacement-side">

          <div className="card algorithm-info">

            <div className="section-header">
              <div>
                <h2>{algorithm} Algorithm</h2>
                <p>Current replacement strategy</p>
              </div>
            </div>

            {algorithm==="FIFO" && (
              <p>
                FIFO replaces the page that has been in memory for
                the longest period of time. It uses a queue-based
                replacement order.
              </p>
            )}

            {algorithm==="LRU" && (
              <p>
                LRU replaces the page that has not been used for
                the longest period of time. Recently accessed pages
                remain in memory.
              </p>
            )}

            {algorithm==="Optimal" && (
              <p>
                Optimal replaces the page whose next use occurs
                farthest in the future. It provides a theoretical
                minimum number of page faults.
              </p>
            )}

            <div className="complexity-box">

              <div>
                <span>Strategy</span>
                <strong>
                  {algorithm==="FIFO"
                    ?"Queue"
                    :algorithm==="LRU"
                    ?"Recent Usage"
                    :"Future Reference"}
                </strong>
              </div>

              <div>
                <span>Frames</span>
                <strong>{frameCount}</strong>
              </div>

            </div>

          </div>

          <div className="card legend-card">

            <div className="section-header">
              <div>
                <h2>Legend</h2>
                <p>Simulation states</p>
              </div>
            </div>

            <div className="replacement-legend">

              <div>
                <span className="legend-square hit"></span>
                <span>Page Hit</span>
              </div>

              <div>
                <span className="legend-square fault"></span>
                <span>Page Fault</span>
              </div>

              <div>
                <span className="legend-square frame"></span>
                <span>Active Frame</span>
              </div>

            </div>

          </div>

        </div>

      </div>

    </div>
  );
}

export default PageReplacement;