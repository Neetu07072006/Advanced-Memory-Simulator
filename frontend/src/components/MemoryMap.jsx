function MemoryMap() {
  const blocks = [
    { name: "P101", size: 150, type: "process" },
    { name: "FREE", size: 220, type: "free" },
    { name: "P102", size: 180, type: "process" },
    { name: "FREE", size: 120, type: "free" },
    { name: "P103", size: 210, type: "process" },
    { name: "FREE", size: 120, type: "free" }
  ];

  return (
    <div className="memory-card card">
      <div className="section-header">
        <div>
          <h2>Physical Memory</h2>
          <p>Current 1000 KB memory layout</p>
        </div>
        <span className="live-badge">LIVE</span>
      </div>

      <div className="memory-map">
        {blocks.map((block, index) => (
          <div
            key={index}
            className={`memory-block ${block.type}`}
            style={{ flex: block.size }}
          >
            <strong>{block.name}</strong>
            <span>{block.size} KB</span>
          </div>
        ))}
      </div>

      <div className="memory-scale">
        <span>0 KB</span>
        <span>250 KB</span>
        <span>500 KB</span>
        <span>750 KB</span>
        <span>1000 KB</span>
      </div>
    </div>
  );
}

export default MemoryMap;