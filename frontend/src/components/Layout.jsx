import TabBar from "./TabBar";

function Layout({ tabs, activeTab, setActiveTab, children }) {
  return (
    <div className="app-shell">
      <header className="top-header">
        <div className="brand-section">
          <div className="brand-mark">M</div>

          <div>
            <h1>Advanced Memory Simulator</h1>
            <p>Operating System Simulation Platform</p>
          </div>
        </div>

        <div className="system-status">
          <span className="status-dot"></span>
          System Active
        </div>
      </header>

      <TabBar
        tabs={tabs}
        activeTab={activeTab}
        setActiveTab={setActiveTab}
      />

      <main className="main-content">
        {children}
      </main>
    </div>
  );
}

export default Layout;