import { useState } from "react";
import { SimulatorProvider } from "./context/SimulatorContext";
import Layout from "./components/Layout";

import Overview from "./pages/Overview";
import PageReplacement from "./pages/PageReplacement";
import VirtualMemory from "./pages/VirtualMemory";
import PhysicalAllocator from "./pages/PhysicalAllocator";
import ProcessManagement from "./pages/ProcessManagement";
import MemoryProtection from "./pages/MemoryProtection";
import SwapManagement from "./pages/SwapManagement";
import CPUScheduler from "./pages/CPUScheduler";
import Analytics from "./pages/Analytics";

function AppContent() {
  const [activeTab, setActiveTab] = useState("overview");

  const tabs = [
    { id: "overview", label: "Overview" },
    { id: "replacement", label: "Page Replacement" },
    { id: "virtual", label: "Virtual Memory & TLB" },
    { id: "allocator", label: "Memory Allocator" },
    { id: "process", label: "Process Management" },
    { id: "protection", label: "Memory Protection" },
    { id: "swap", label: "Swap Management" },
    { id: "scheduler", label: "CPU Scheduler" },
    { id: "analytics", label: "Analytics" },
  ];

  return (
    <Layout
      tabs={tabs}
      activeTab={activeTab}
      setActiveTab={setActiveTab}
    >
      {activeTab === "overview" && <Overview />}
      {activeTab === "replacement" && <PageReplacement />}
      {activeTab === "virtual" && <VirtualMemory />}
      {activeTab === "allocator" && <PhysicalAllocator />}
      {activeTab === "process" && <ProcessManagement />}
      {activeTab === "protection" && <MemoryProtection />}
      {activeTab === "swap" && <SwapManagement />}
      {activeTab === "scheduler" && <CPUScheduler />}
      {activeTab === "analytics" && <Analytics />}
    </Layout>
  );
}

function App() {
  return (
    <SimulatorProvider>
      <AppContent />
    </SimulatorProvider>
  );
}

export default App;