import { createContext, useContext, useMemo, useState } from "react";

const SimulatorContext = createContext(null);

const initialState = {
  memory: {
    total: 1000,
    used: 620,
    free: 380,
    fragmentation: 14
  },
  virtualMemory: {
    translations: 50,
    tlbHits: 43,
    tlbMisses: 7,
    pageFaults: 9
  },
  swap: {
    totalSlots: 8,
    usedSlots: 3
  },
  cpu: {
    utilization: 82,
    processCount: 4,
    averageWaitingTime: 4.75,
    averageTurnaroundTime: 10.25
  },
  activity: []
};

export function SimulatorProvider({ children }) {
  const [state, setState] = useState(initialState);

  const updateMemory = (data) => {
    setState(prev => ({
      ...prev,
      memory: {
        ...prev.memory,
        ...data
      }
    }));
  };

  const recordTranslation = ({ hit = false, pageFault = false } = {}) => {
    setState(prev => ({
      ...prev,
      virtualMemory: {
        ...prev.virtualMemory,
        translations: prev.virtualMemory.translations + 1,
        tlbHits: prev.virtualMemory.tlbHits + (hit ? 1 : 0),
        tlbMisses: prev.virtualMemory.tlbMisses + (hit ? 0 : 1),
        pageFaults: prev.virtualMemory.pageFaults + (pageFault ? 1 : 0)
      }
    }));
  };

  const updateVirtualMemory = (data) => {
    setState(prev => ({
      ...prev,
      virtualMemory: {
        ...prev.virtualMemory,
        ...data
      }
    }));
  };

  const updateSwap = (data) => {
    setState(prev => ({
      ...prev,
      swap: {
        ...prev.swap,
        ...data
      }
    }));
  };

  const updateCPU = (data) => {
    setState(prev => ({
      ...prev,
      cpu: {
        ...prev.cpu,
        ...data
      }
    }));
  };

  const addActivity = (message, type = "info") => {
    setState(prev => ({
      ...prev,
      activity: [
        {
          id: Date.now(),
          message,
          type,
          time: new Date().toLocaleTimeString()
        },
        ...prev.activity
      ].slice(0, 20)
    }));
  };

  const resetSimulator = () => {
    setState(initialState);
  };

  const metrics = useMemo(() => {
    const memoryUtilization =
      state.memory.total > 0
        ? (state.memory.used / state.memory.total) * 100
        : 0;

    const tlbHitRate =
      state.virtualMemory.translations > 0
        ? (state.virtualMemory.tlbHits / state.virtualMemory.translations) * 100
        : 0;

    const pageFaultRate =
      state.virtualMemory.translations > 0
        ? (state.virtualMemory.pageFaults / state.virtualMemory.translations) * 100
        : 0;

    const swapUtilization =
      state.swap.totalSlots > 0
        ? (state.swap.usedSlots / state.swap.totalSlots) * 100
        : 0;

    return {
      memoryUtilization: Number(memoryUtilization.toFixed(1)),
      tlbHitRate: Number(tlbHitRate.toFixed(1)),
      pageFaultRate: Number(pageFaultRate.toFixed(1)),
      fragmentation: state.memory.fragmentation,
      swapUtilization: Number(swapUtilization.toFixed(1)),
      cpuUtilization: state.cpu.utilization,
      avgWaitingTime: state.cpu.averageWaitingTime,
      avgTurnaroundTime: state.cpu.averageTurnaroundTime
    };
  }, [state]);

  const value = {
    state,
    metrics,
    updateMemory,
    recordTranslation,
    updateVirtualMemory,
    updateSwap,
    updateCPU,
    addActivity,
    resetSimulator
  };

  return (
    <SimulatorContext.Provider value={value}>
      {children}
    </SimulatorContext.Provider>
  );
}

export function useSimulator() {
  const context = useContext(SimulatorContext);

  if (!context) {
    throw new Error(
      "useSimulator must be used inside SimulatorProvider"
    );
  }

  return context;
}