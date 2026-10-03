import { useMemo, useState } from "react";

const initialRegions=[
  {id:1,name:"Process A",start:0,end:119,owner:"P1",read:true,write:true,execute:false,status:"Protected"},
  {id:2,name:"Process B",start:200,end:379,owner:"P2",read:true,write:true,execute:false,status:"Protected"},
  {id:3,name:"Process C",start:500,end:719,owner:"P3",read:true,write:false,execute:true,status:"Protected"},
  {id:4,name:"Process D",start:820,end:919,owner:"P4",read:true,write:true,execute:false,status:"Protected"}
];

function MemoryProtection(){
  const [regions,setRegions]=useState(initialRegions);
  const [address,setAddress]=useState("");
  const [operation,setOperation]=useState("Read");
  const [selected,setSelected]=useState(null);
  const [result,setResult]=useState(null);
  const [accessLog,setAccessLog]=useState([]);

  const stats=useMemo(()=>({
    regions:regions.length,
    protected:regions.filter(r=>r.status==="Protected").length,
    violations:accessLog.filter(x=>x.type==="violation").length,
    allowed:accessLog.filter(x=>x.type==="allowed").length
  }),[regions,accessLog]);

  const findRegion=addr=>{
    return regions.find(r=>addr>=r.start&&addr<=r.end);
  };

  const checkAccess=()=>{
    const addr=Number(address);

    if(address===""||Number.isNaN(addr)||addr<0||addr>999){
      setResult({
        type:"error",
        title:"Invalid Address",
        text:"Enter a physical address between 0 and 999 KB."
      });
      return;
    }

    const region=findRegion(addr);

    if(!region){
      const entry={
        id:Date.now(),
        address:addr,
        operation,
        process:"Unknown",
        type:"violation",
        reason:"Address does not belong to a protected region."
      };

      setAccessLog(prev=>[entry,...prev].slice(0,8));
      setResult({
        type:"violation",
        title:"Access Violation",
        text:`Address ${addr} KB does not belong to an allocated protected region.`
      });
      return;
    }

    const allowed=
      operation==="Read"?region.read:
      operation==="Write"?region.write:
      region.execute;

    const entry={
      id:Date.now(),
      address:addr,
      operation,
      process:region.owner,
      type:allowed?"allowed":"violation",
      reason:allowed?"Permission granted":"Permission denied"
    };

    setAccessLog(prev=>[entry,...prev].slice(0,8));

    if(allowed){
      setResult({
        type:"allowed",
        title:"Access Granted",
        text:`${operation} access allowed for ${region.name} (${region.owner}) at address ${addr} KB.`
      });
    }else{
      setResult({
        type:"violation",
        title:"Access Violation",
        text:`${operation} access denied for ${region.name} (${region.owner}).`
      });
    }
  };

  const togglePermission=(id,permission)=>{
    setRegions(prev=>prev.map(r=>{
      if(r.id!==id)return r;
      return {...r,[permission]:!r[permission]};
    }));
  };

  const reset=()=>{
    setRegions(initialRegions);
    setAddress("");
    setOperation("Read");
    setSelected(null);
    setResult(null);
    setAccessLog([]);
  };

  return(
    <section className="page-section">
      <div className="page-heading">
        <div>
          <span className="eyebrow">MEMORY SECURITY</span>
          <h2>Memory Protection</h2>
          <p>Control process memory permissions and detect illegal memory access.</p>
        </div>

        <div className="allocator-capacity">
          <span>Protection System</span>
          <strong>Active</strong>
        </div>
      </div>

      <div className="protection-stats">
        <div className="protection-stat">
          <span>Protected Regions</span>
          <strong>{stats.regions}</strong>
          <small>Registered memory areas</small>
        </div>

        <div className="protection-stat">
          <span>Protected</span>
          <strong>{stats.protected}</strong>
          <small>Active protection rules</small>
        </div>

        <div className="protection-stat">
          <span>Allowed Access</span>
          <strong>{stats.allowed}</strong>
          <small>Successful operations</small>
        </div>

        <div className="protection-stat">
          <span>Violations</span>
          <strong className={stats.violations>0?"danger-number":""}>
            {stats.violations}
          </strong>
          <small>Blocked operations</small>
        </div>
      </div>

      <div className="protection-main-grid">
        <div className="protection-check-card">
          <div className="card-heading">
            <div>
              <h3>Memory Access Checker</h3>
              <p>Test whether a process is allowed to access a memory address.</p>
            </div>
            <span className="protection-icon">✓</span>
          </div>

          <div className="protection-form">
            <label>
              <span>Memory Address</span>
              <div className="address-input">
                <input
                  type="number"
                  min="0"
                  max="999"
                  value={address}
                  onChange={e=>setAddress(e.target.value)}
                  placeholder="0 - 999"
                />
                <b>KB</b>
              </div>
            </label>

            <label>
              <span>Operation</span>
              <select value={operation} onChange={e=>setOperation(e.target.value)}>
                <option>Read</option>
                <option>Write</option>
                <option>Execute</option>
              </select>
            </label>

            <button className="primary-button" onClick={checkAccess}>
              Check Access
            </button>
          </div>

          {result&&(
            <div className={`access-result ${result.type}`}>
              <div className="result-symbol">
                {result.type==="allowed"?"✓":"!"}
              </div>
              <div>
                <strong>{result.title}</strong>
                <span>{result.text}</span>
              </div>
            </div>
          )}

          <div className="protection-rule">
            <strong>Protection Rule</strong>
            <span>
              Every memory access must match the permissions assigned to the
              process-owned memory region.
            </span>
          </div>
        </div>

        <div className="protection-legend-card">
          <h3>Permission Model</h3>

          <div className="permission-item">
            <div className="permission-symbol read">R</div>
            <div>
              <strong>Read</strong>
              <span>Process can read data.</span>
            </div>
          </div>

          <div className="permission-item">
            <div className="permission-symbol write">W</div>
            <div>
              <strong>Write</strong>
              <span>Process can modify data.</span>
            </div>
          </div>

          <div className="permission-item">
            <div className="permission-symbol execute">X</div>
            <div>
              <strong>Execute</strong>
              <span>Process can execute instructions.</span>
            </div>
          </div>

          <button className="secondary-button protection-reset" onClick={reset}>
            Reset Protection
          </button>
        </div>
      </div>

      <div className="protection-region-card">
        <div className="card-heading">
          <div>
            <h3>Protected Memory Regions</h3>
            <p>Configure read, write and execute permissions for each region.</p>
          </div>
        </div>

        <div className="region-table">
          <div className="region-table-head">
            <span>Region</span>
            <span>Owner</span>
            <span>Address Range</span>
            <span>Read</span>
            <span>Write</span>
            <span>Execute</span>
            <span>Status</span>
          </div>

          {regions.map(region=>(
            <div
              className={`region-table-row ${selected===region.id?"region-selected":""}`}
              key={region.id}
              onClick={()=>setSelected(region.id)}
            >
              <span className="region-name">{region.name}</span>
              <span className="owner-badge">{region.owner}</span>
              <span>{region.start} - {region.end} KB</span>

              <span>
                <button
                  className={`permission-toggle ${region.read?"enabled":""}`}
                  onClick={e=>{
                    e.stopPropagation();
                    togglePermission(region.id,"read");
                  }}
                >
                  {region.read?"✓":"×"}
                </button>
              </span>

              <span>
                <button
                  className={`permission-toggle ${region.write?"enabled":""}`}
                  onClick={e=>{
                    e.stopPropagation();
                    togglePermission(region.id,"write");
                  }}
                >
                  {region.write?"✓":"×"}
                </button>
              </span>

              <span>
                <button
                  className={`permission-toggle ${region.execute?"enabled":""}`}
                  onClick={e=>{
                    e.stopPropagation();
                    togglePermission(region.id,"execute");
                  }}
                >
                  {region.execute?"✓":"×"}
                </button>
              </span>

              <span>
                <b className="protected-badge">Protected</b>
              </span>
            </div>
          ))}
        </div>
      </div>

      <div className="protection-bottom-grid">
        <div className="access-log-card">
          <div className="card-heading">
            <div>
              <h3>Access Log</h3>
              <p>Recent memory access attempts.</p>
            </div>
          </div>

          {accessLog.length===0?(
            <div className="empty-access-log">
              No memory access attempts yet.
            </div>
          ):(
            <div className="access-log">
              {accessLog.map(log=>(
                <div className="access-log-row" key={log.id}>
                  <span className={`log-icon ${log.type}`}>
                    {log.type==="allowed"?"✓":"!"}
                  </span>
                  <div>
                    <strong>
                      {log.operation} at {log.address} KB
                    </strong>
                    <small>
                      {log.process} · {log.reason}
                    </small>
                  </div>
                  <b className={log.type}>
                    {log.type==="allowed"?"ALLOWED":"BLOCKED"}
                  </b>
                </div>
              ))}
            </div>
          )}
        </div>

        <div className="protection-info-card">
          <h3>How Protection Works</h3>

          <div className="protection-flow">
            <div>
              <strong>1</strong>
              <span>Process requests memory access</span>
            </div>
            <i>↓</i>
            <div>
              <strong>2</strong>
              <span>Find owning memory region</span>
            </div>
            <i>↓</i>
            <div>
              <strong>3</strong>
              <span>Check R / W / X permission</span>
            </div>
            <i>↓</i>
            <div>
              <strong>4</strong>
              <span>Allow or raise violation</span>
            </div>
          </div>
        </div>
      </div>
    </section>
  );
}

export default MemoryProtection;