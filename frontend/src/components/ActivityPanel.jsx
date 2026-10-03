function ActivityPanel({ running }) {
  const events = [
    ["P101", "allocated 150 KB", "blue"],
    ["P102", "entered RUNNING state", "green"],
    ["Page 4", "page fault detected", "orange"],
    ["P103", "entered WAITING state", "purple"],
    ["P101", "memory access validated", "blue"]
  ];

  return (
    <div className="activity-card card">
      <div className="section-header">
        <div>
          <h2>System Activity</h2>
          <p>
            {running
              ? "Live simulation events"
              : "Recent simulator events"}
          </p>
        </div>

        <span className="event-count">
          {events.length}
        </span>
      </div>

      <div className="activity-list">
        {events.map((event, index) => (
          <div className="activity-item" key={index}>
            <span className={`activity-dot ${event[2]}`}></span>

            <div>
              <strong>{event[0]}</strong>
              <span>{event[1]}</span>
            </div>

            <time>
              {running ? `${index + 1}s` : "—"}
            </time>
          </div>
        ))}
      </div>
    </div>
  );
}

export default ActivityPanel;