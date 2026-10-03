import { useSimulator } from "../context/SimulatorContext";

function Analytics() {
  const { metrics, state } = useSimulator();

  const memoryTrend = [
    Math.max(0, metrics.memoryUtilization - 14),
    Math.max(0, metrics.memoryUtilization - 10),
    Math.max(0, metrics.memoryUtilization - 7),
    Math.max(0, metrics.memoryUtilization - 4),
    metrics.memoryUtilization,
    Math.max(0, metrics.memoryUtilization - 3),
    Math.min(100, metrics.memoryUtilization + 2),
    metrics.memoryUtilization
  ];

  const pageFaults = [
    4,
    7,
    5,
    9,
    6,
    11,
    8,
    Math.max(1, state.virtualMemory.pageFaults)
  ];

  const cpuTrend = [
    Math.max(0, metrics.cpuUtilization - 18),
    Math.max(0, metrics.cpuUtilization - 10),
    Math.max(0, metrics.cpuUtilization - 14),
    Math.min(100, metrics.cpuUtilization - 1),
    Math.max(0, metrics.cpuUtilization - 6),
    Math.min(100, metrics.cpuUtilization + 6),
    metrics.cpuUtilization,
    Math.min(100, metrics.cpuUtilization + 2)
  ];

  const fragmentationTrend = [
    22,
    19,
    21,
    17,
    16,
    18,
    Math.max(0, metrics.fragmentation),
    metrics.fragmentation
  ];

  const memoryEfficiency = Math.max(
    0,
    Math.round(100 - metrics.fragmentation)
  );

  const allocationEfficiency = Math.max(
    0,
    Math.round(100 - metrics.fragmentation)
  );

  return (
    <div className="page analytics-page">

      <div className="page-heading">
        <div>
          <span className="eyebrow">PERFORMANCE CENTER</span>

          <h2>Analytics & Performance</h2>

          <p>
            Monitor memory, paging, allocation, swapping and CPU performance
            from a single dashboard.
          </p>
        </div>

        <div className="analytics-live">
          <span></span>
          Live Simulation Data
        </div>
      </div>

      <div className="analytics-stat-grid">

        <div className="analytics-stat">
          <div className="analytics-stat-top">
            <span>Memory Utilization</span>
            <strong>{metrics.memoryUtilization}%</strong>
          </div>

          <div className="analytics-progress">
            <div
              style={{
                width: `${metrics.memoryUtilization}%`
              }}
            ></div>
          </div>

          <small>Physical memory usage</small>
        </div>

        <div className="analytics-stat">
          <div className="analytics-stat-top">
            <span>TLB Hit Rate</span>
            <strong>{metrics.tlbHitRate}%</strong>
          </div>

          <div className="analytics-progress">
            <div
              style={{
                width: `${metrics.tlbHitRate}%`
              }}
            ></div>
          </div>

          <small>Address translation efficiency</small>
        </div>

        <div className="analytics-stat">
          <div className="analytics-stat-top">
            <span>Page Fault Rate</span>
            <strong>{metrics.pageFaultRate}%</strong>
          </div>

          <div className="analytics-progress">
            <div
              style={{
                width: `${Math.min(100, metrics.pageFaultRate)}%`
              }}
            ></div>
          </div>

          <small>Virtual memory faults</small>
        </div>

        <div className="analytics-stat">
          <div className="analytics-stat-top">
            <span>Fragmentation</span>
            <strong>{metrics.fragmentation}%</strong>
          </div>

          <div className="analytics-progress">
            <div
              style={{
                width: `${metrics.fragmentation}%`
              }}
            ></div>
          </div>

          <small>External fragmentation</small>
        </div>

        <div className="analytics-stat">
          <div className="analytics-stat-top">
            <span>Swap Utilization</span>
            <strong>{metrics.swapUtilization}%</strong>
          </div>

          <div className="analytics-progress">
            <div
              style={{
                width: `${metrics.swapUtilization}%`
              }}
            ></div>
          </div>

          <small>Swap space consumption</small>
        </div>

        <div className="analytics-stat">
          <div className="analytics-stat-top">
            <span>CPU Utilization</span>
            <strong>{metrics.cpuUtilization}%</strong>
          </div>

          <div className="analytics-progress">
            <div
              style={{
                width: `${metrics.cpuUtilization}%`
              }}
            ></div>
          </div>

          <small>Processor utilization</small>
        </div>

        <div className="analytics-stat">
          <div className="analytics-stat-top">
            <span>Avg Waiting Time</span>
            <strong>{metrics.avgWaitingTime}</strong>
          </div>

          <div className="analytics-number">
            <span>time units</span>
          </div>

          <small>CPU scheduling performance</small>
        </div>

        <div className="analytics-stat">
          <div className="analytics-stat-top">
            <span>Avg Turnaround</span>
            <strong>{metrics.avgTurnaroundTime}</strong>
          </div>

          <div className="analytics-number">
            <span>time units</span>
          </div>

          <small>Process completion efficiency</small>
        </div>

      </div>

      <div className="analytics-grid">

        <div className="analytics-card">

          <div className="analytics-card-header">
            <div>
              <h3>Memory Utilization</h3>
              <span>Recent simulation activity</span>
            </div>

            <strong>{metrics.memoryUtilization}%</strong>
          </div>

          <div className="analytics-line-chart">

            <div className="analytics-chart-grid">
              <span>100</span>
              <span>75</span>
              <span>50</span>
              <span>25</span>
              <span>0</span>
            </div>

            <div className="analytics-chart-bars">

              {memoryTrend.map((value, index) => (
                <div
                  className="analytics-chart-column"
                  key={index}
                >
                  <div
                    className="analytics-chart-bar"
                    style={{
                      "--bar-height": `${value}%`
                    }}
                  ></div>

                  <span>{index + 1}</span>
                </div>
              ))}

            </div>

          </div>

        </div>

        <div className="analytics-card">

          <div className="analytics-card-header">
            <div>
              <h3>CPU Utilization</h3>
              <span>Scheduler activity</span>
            </div>

            <strong>{metrics.cpuUtilization}%</strong>
          </div>

          <div className="analytics-line-chart">

            <div className="analytics-chart-grid">
              <span>100</span>
              <span>75</span>
              <span>50</span>
              <span>25</span>
              <span>0</span>
            </div>

            <div className="analytics-chart-bars">

              {cpuTrend.map((value, index) => (
                <div
                  className="analytics-chart-column"
                  key={index}
                >
                  <div
                    className="analytics-chart-bar analytics-cpu-bar"
                    style={{
                      "--bar-height": `${value}%`
                    }}
                  ></div>

                  <span>{index + 1}</span>
                </div>
              ))}

            </div>

          </div>

        </div>

        <div className="analytics-card">

          <div className="analytics-card-header">
            <div>
              <h3>Page Faults</h3>
              <span>Paging activity</span>
            </div>

            <strong>{state.virtualMemory.pageFaults}</strong>
          </div>

          <div className="analytics-mini-chart">

            {pageFaults.map((value, index) => (
              <div
                className="analytics-mini-column"
                key={index}
              >
                <div
                  className="analytics-mini-bar"
                  style={{
                    "--bar-height": `${Math.min(
                      100,
                      value * 7
                    )}%`
                  }}
                ></div>

                <span>{index + 1}</span>
              </div>
            ))}

          </div>

        </div>

        <div className="analytics-card">

          <div className="analytics-card-header">
            <div>
              <h3>Fragmentation</h3>
              <span>Allocator efficiency</span>
            </div>

            <strong>{metrics.fragmentation}%</strong>
          </div>

          <div className="analytics-mini-chart">

            {fragmentationTrend.map((value, index) => (
              <div
                className="analytics-mini-column"
                key={index}
              >
                <div
                  className="analytics-mini-bar analytics-fragmentation-bar"
                  style={{
                    "--bar-height": `${Math.min(
                      100,
                      value * 4
                    )}%`
                  }}
                ></div>

                <span>{index + 1}</span>
              </div>
            ))}

          </div>

        </div>

      </div>

      <div className="analytics-bottom-grid">

        <div className="analytics-card health-card">

          <div className="analytics-card-header">
            <div>
              <h3>System Health</h3>
              <span>Current simulator status</span>
            </div>

            <span className="health-badge">
              Operational
            </span>
          </div>

          <div className="health-list">

            <div>
              <span className="health-dot"></span>

              <div>
                <strong>Memory Manager</strong>
                <small>
                  {state.memory.used} KB used of {state.memory.total} KB
                </small>
              </div>

              <b>Active</b>
            </div>

            <div>
              <span className="health-dot"></span>

              <div>
                <strong>Virtual Memory</strong>
                <small>
                  {state.virtualMemory.translations} translations recorded
                </small>
              </div>

              <b>Active</b>
            </div>

            <div>
              <span className="health-dot"></span>

              <div>
                <strong>Swap Manager</strong>
                <small>
                  {state.swap.usedSlots} of {state.swap.totalSlots} slots used
                </small>
              </div>

              <b>Active</b>
            </div>

            <div>
              <span className="health-dot"></span>

              <div>
                <strong>CPU Scheduler</strong>
                <small>
                  {state.cpu.processCount} processes in simulation
                </small>
              </div>

              <b>Active</b>
            </div>

          </div>

        </div>

        <div className="analytics-card performance-card">

          <div className="analytics-card-header">
            <div>
              <h3>Performance Summary</h3>
              <span>Key simulator indicators</span>
            </div>
          </div>

          <div className="performance-list">

            <div>
              <span>Memory Efficiency</span>
              <strong>{memoryEfficiency}%</strong>
            </div>

            <div>
              <span>Translation Efficiency</span>
              <strong>{metrics.tlbHitRate}%</strong>
            </div>

            <div>
              <span>Allocation Efficiency</span>
              <strong>{allocationEfficiency}%</strong>
            </div>

            <div>
              <span>Scheduling Efficiency</span>
              <strong>{metrics.cpuUtilization}%</strong>
            </div>

          </div>

        </div>

      </div>

      <div className="analytics-info">

        <div className="analytics-info-icon">
          i
        </div>

        <div>
          <strong>Analytics Architecture</strong>

          <p>
            Analytics is connected to the global simulator state.
            Virtual memory translations, TLB activity, page faults,
            memory usage, swap utilization and CPU metrics can now
            be shared across simulator modules.
          </p>
        </div>

      </div>

    </div>
  );
}

export default Analytics;