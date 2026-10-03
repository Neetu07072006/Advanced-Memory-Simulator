import { useMemo, useState } from "react";
import { useSimulator } from "../context/SimulatorContext";

const TOTAL_MEMORY = 1000;

const initialBlocks = [
  { id: 1, start: 0, size: 120, free: false, name: "Process A" },
  { id: 2, start: 120, size: 80, free: true, name: "Free" },
  { id: 3, start: 200, size: 180, free: false, name: "Process B" },
  { id: 4, start: 380, size: 120, free: true, name: "Free" },
  { id: 5, start: 500, size: 220, free: false, name: "Process C" },
  { id: 6, start: 720, size: 100, free: true, name: "Free" },
  { id: 7, start: 820, size: 100, free: false, name: "Process D" },
  { id: 8, start: 920, size: 80, free: true, name: "Free" }
];

function PhysicalAllocator() {
  const { updateMemory, addActivity } = useSimulator();

  const [blocks, setBlocks] = useState(initialBlocks);
  const [strategy, setStrategy] = useState("First Fit");
  const [processName, setProcessName] = useState("");
  const [memorySize, setMemorySize] = useState("");
  const [selectedBlock, setSelectedBlock] = useState(null);
  const [history, setHistory] = useState([]);
  const [nextId, setNextId] = useState(9);
  const [nextFitPointer, setNextFitPointer] = useState(0);
  const [message, setMessage] = useState({
    type: "info",
    text: "Ready for memory allocation."
  });

  const usedMemory = useMemo(
    () =>
      blocks
        .filter(block => !block.free)
        .reduce((sum, block) => sum + block.size, 0),
    [blocks]
  );

  const freeMemory = TOTAL_MEMORY - usedMemory;

  const freeBlocks = blocks.filter(block => block.free);

  const largestFreeBlock = freeBlocks.length
    ? Math.max(...freeBlocks.map(block => block.size))
    : 0;

  const fragmentation =
    freeMemory > 0
      ? Math.round(
          ((freeMemory - largestFreeBlock) / freeMemory) * 100
        )
      : 0;

  const updateGlobalMemory = (
    updatedBlocks,
    actionMessage,
    actionType = "info"
  ) => {
    const used = updatedBlocks
      .filter(block => !block.free)
      .reduce((sum, block) => sum + block.size, 0);

    const free = TOTAL_MEMORY - used;

    const freeList = updatedBlocks.filter(block => block.free);

    const largest =
      freeList.length > 0
        ? Math.max(...freeList.map(block => block.size))
        : 0;

    const frag =
      free > 0
        ? Math.round(((free - largest) / free) * 100)
        : 0;

    updateMemory({
      total: TOTAL_MEMORY,
      used,
      free,
      fragmentation: frag
    });

    addActivity(actionMessage, actionType);
  };

  const chooseBlock = (size, currentBlocks = blocks) => {
    const candidates = currentBlocks.filter(
      block => block.free && block.size >= size
    );

    if (!candidates.length) {
      return null;
    }

    if (strategy === "First Fit") {
      return candidates[0];
    }

    if (strategy === "Best Fit") {
      return candidates.reduce((best, block) =>
        block.size < best.size ? block : best
      );
    }

    if (strategy === "Worst Fit") {
      return candidates.reduce((worst, block) =>
        block.size > worst.size ? block : worst
      );
    }

    if (strategy === "Next Fit") {
      const ordered = [...currentBlocks].sort(
        (a, b) => a.start - b.start
      );

      const startIndex = ordered.findIndex(
        block => block.id === nextFitPointer
      );

      const actualStart = startIndex >= 0 ? startIndex : 0;

      for (let i = 0; i < ordered.length; i++) {
        const index =
          (actualStart + i) % ordered.length;

        const block = ordered[index];

        if (block.free && block.size >= size) {
          return block;
        }
      }

      return null;
    }

    return candidates[0];
  };

  const allocateMemory = () => {
    const size = Number(memorySize);
    const name = processName.trim();

    if (!name) {
      setMessage({
        type: "error",
        text: "Enter a process name."
      });
      return;
    }

    if (!size || size <= 0) {
      setMessage({
        type: "error",
        text: "Enter a valid memory size."
      });
      return;
    }

    if (size > freeMemory) {
      setMessage({
        type: "error",
        text: "Not enough total free memory."
      });
      return;
    }

    const target = chooseBlock(size);

    if (!target) {
      setMessage({
        type: "error",
        text: `${strategy} could not find a suitable free block.`
      });
      return;
    }

    const newBlocks = [];

    blocks.forEach(block => {
      if (block.id !== target.id) {
        newBlocks.push(block);
        return;
      }

      const allocatedBlock = {
        id: nextId,
        start: block.start,
        size,
        free: false,
        name
      };

      newBlocks.push(allocatedBlock);

      const remainingSize = block.size - size;

      if (remainingSize > 0) {
        newBlocks.push({
          id: nextId + 1,
          start: block.start + size,
          size: remainingSize,
          free: true,
          name: "Free"
        });
      }
    });

    const sortedBlocks = newBlocks.sort(
      (a, b) => a.start - b.start
    );

    setBlocks(sortedBlocks);
    setNextId(nextId + 2);

    if (strategy === "Next Fit") {
      const nextFree = sortedBlocks.find(
        block =>
          block.free &&
          block.start >= target.start + size
      );

      setNextFitPointer(
        nextFree
          ? nextFree.id
          : sortedBlocks.find(block => block.free)?.id || 0
      );
    }

    setHistory(prev => [
      {
        id: Date.now(),
        type: "allocate",
        process: name,
        size,
        strategy,
        time: new Date().toLocaleTimeString()
      },
      ...prev
    ]);

    setMessage({
      type: "success",
      text: `${name} allocated ${size} KB using ${strategy}.`
    });

    updateGlobalMemory(
      sortedBlocks,
      `${name} allocated ${size} KB using ${strategy}.`,
      "success"
    );

    setProcessName("");
    setMemorySize("");
  };

  const deallocateMemory = () => {
    if (!selectedBlock) {
      setMessage({
        type: "error",
        text: "Select an allocated memory block first."
      });
      return;
    }

    const block = blocks.find(
      item => item.id === selectedBlock
    );

    if (!block || block.free) {
      setMessage({
        type: "error",
        text: "The selected block is not allocated."
      });
      return;
    }

    let updatedBlocks = blocks.map(item =>
      item.id === block.id
        ? {
            ...item,
            free: true,
            name: "Free"
          }
        : item
    );

    updatedBlocks = updatedBlocks.sort(
      (a, b) => a.start - b.start
    );

    const merged = [];

    updatedBlocks.forEach(item => {
      const previous = merged[merged.length - 1];

      if (
        previous &&
        previous.free &&
        item.free &&
        previous.start + previous.size === item.start
      ) {
        previous.size += item.size;
      } else {
        merged.push({ ...item });
      }
    });

    setBlocks(merged);
    setSelectedBlock(null);

    setHistory(prev => [
      {
        id: Date.now(),
        type: "deallocate",
        process: block.name,
        size: block.size,
        strategy: "Deallocation",
        time: new Date().toLocaleTimeString()
      },
      ...prev
    ]);

    setMessage({
      type: "success",
      text: `${block.name} was deallocated and adjacent free blocks were merged.`
    });

    updateGlobalMemory(
      merged,
      `${block.name} deallocated ${block.size} KB.`,
      "info"
    );
  };

  const resetMemory = () => {
    setBlocks(initialBlocks);
    setSelectedBlock(null);
    setHistory([]);
    setNextId(9);
    setNextFitPointer(0);

    updateMemory({
      total: TOTAL_MEMORY,
      used: 620,
      free: 380,
      fragmentation: 14
    });

    addActivity(
      "Physical memory allocator was reset.",
      "info"
    );

    setMessage({
      type: "info",
      text: "Memory allocator has been reset."
    });
  };

  const selectBlock = block => {
    if (!block.free) {
      setSelectedBlock(block.id);
    }
  };

  return (
    <div className="page">

      <div className="page-heading">
        <div>
          <span className="eyebrow">
            MEMORY MANAGEMENT
          </span>

          <h2>Physical Memory Allocator</h2>

          <p>
            Simulate dynamic memory allocation using
            First Fit, Best Fit, Worst Fit and Next Fit.
          </p>
        </div>

        <div className="allocator-capacity">
          <span>Total Memory</span>
          <strong>{TOTAL_MEMORY} KB</strong>
        </div>
      </div>

      <div className="allocator-message">
        <span
          className={
            message.type === "error"
              ? "message-error"
              : message.type === "success"
                ? "message-success"
                : ""
          }
        >
          {message.text}
        </span>
      </div>

      <div className="allocator-controls">

        <div className="allocator-control">
          <label>Allocation Strategy</label>

          <div className="strategy-buttons">
            {[
              "First Fit",
              "Best Fit",
              "Worst Fit",
              "Next Fit"
            ].map(item => (
              <button
                key={item}
                className={
                  strategy === item
                    ? "active"
                    : ""
                }
                onClick={() => setStrategy(item)}
              >
                {item}
              </button>
            ))}
          </div>
        </div>

        <div className="allocator-control">
          <label>Process Name</label>

          <input
            value={processName}
            onChange={e =>
              setProcessName(e.target.value)
            }
            placeholder="Process E"
          />
        </div>

        <div className="allocator-control">
          <label>Memory Required</label>

          <input
            type="number"
            min="1"
            value={memorySize}
            onChange={e =>
              setMemorySize(e.target.value)
            }
            placeholder="100 KB"
          />
        </div>

        <button
          className="primary-button"
          onClick={allocateMemory}
        >
          Allocate
        </button>

        <button
          className="secondary-button"
          onClick={deallocateMemory}
        >
          Deallocate
        </button>

      </div>

      <div className="allocator-stats">

        <div className="allocator-stat">
          <span>Used Memory</span>
          <strong>{usedMemory} KB</strong>
          <small>
            {Math.round(
              (usedMemory / TOTAL_MEMORY) * 100
            )}
            % utilization
          </small>
        </div>

        <div className="allocator-stat">
          <span>Free Memory</span>
          <strong>{freeMemory} KB</strong>
          <small>
            {100 -
              Math.round(
                (usedMemory / TOTAL_MEMORY) * 100
              )}
            % available
          </small>
        </div>

        <div className="allocator-stat">
          <span>Free Blocks</span>
          <strong>{freeBlocks.length}</strong>
          <small>
            Largest: {largestFreeBlock} KB
          </small>
        </div>

        <div className="allocator-stat">
          <span>Fragmentation</span>
          <strong>{fragmentation}%</strong>
          <small>External fragmentation</small>
        </div>

      </div>

      <div className="allocator-main-grid">

        <div className="memory-map-card">

          <div className="card-header">
            <div>
              <h3>Memory Map</h3>
              <p>
                Select an allocated block to deallocate it
              </p>
            </div>

            <span>
              {usedMemory} / {TOTAL_MEMORY} KB
            </span>
          </div>

          <div className="memory-map-visual">

            {blocks.map(block => {
              const width =
                (block.size / TOTAL_MEMORY) * 100;

              return (
                <button
                  key={block.id}
                  className={`memory-block ${
                    block.free
                      ? "memory-free"
                      : "memory-used"
                  } ${
                    selectedBlock === block.id
                      ? "memory-selected"
                      : ""
                  }`}
                  style={{
                    width: `${width}%`
                  }}
                  onClick={() => selectBlock(block)}
                  title={
                    block.free
                      ? `Free: ${block.size} KB`
                      : `${block.name}: ${block.size} KB`
                  }
                >
                  <span>
                    {block.free
                      ? "Free"
                      : block.name}
                  </span>

                  <small>
                    {block.size} KB
                  </small>
                </button>
              );
            })}

          </div>

          <div className="memory-scale">
            <span>0 KB</span>
            <span>250 KB</span>
            <span>500 KB</span>
            <span>750 KB</span>
            <span>1000 KB</span>
          </div>

          <div className="memory-legend">

            <div>
              <span className="memory-dot used"></span>
              Allocated
            </div>

            <div>
              <span className="memory-dot free"></span>
              Free
            </div>

            <div>
              <span className="memory-dot selected"></span>
              Selected
            </div>

          </div>

        </div>

        <div className="allocator-side">

          <div className="allocator-info">

            <h3>Current Strategy</h3>

            <strong>{strategy}</strong>

            <p>
              {strategy === "First Fit" &&
                "Allocates the first available block large enough for the request."}

              {strategy === "Best Fit" &&
                "Allocates the smallest available block that can satisfy the request."}

              {strategy === "Worst Fit" &&
                "Allocates the largest available block to leave smaller fragments."}

              {strategy === "Next Fit" &&
                "Continues searching from the location of the previous allocation."}
            </p>

          </div>

          {selectedBlock && (
            <div className="selected-process">

              <span>Selected Process</span>

              <strong>
                {
                  blocks.find(
                    block =>
                      block.id === selectedBlock
                  )?.name
                }
              </strong>

              <small>
                {
                  blocks.find(
                    block =>
                      block.id === selectedBlock
                  )?.size
                }{" "}
                KB allocated
              </small>

            </div>
          )}

          <div className="allocator-actions">

            <button
              className="reset-memory-button"
              onClick={resetMemory}
            >
              Reset Memory
            </button>

          </div>

        </div>

      </div>

      <div className="block-table-card">

        <div className="card-header">
          <div>
            <h3>Memory Blocks</h3>
            <p>
              Current physical memory layout
            </p>
          </div>
        </div>

        <div className="block-table">

          <div className="block-table-head">
            <span>Block</span>
            <span>Start</span>
            <span>Size</span>
            <span>Status</span>
            <span>Process</span>
          </div>

          {blocks.map(block => (
            <div
              className="block-table-row"
              key={block.id}
            >
              <span>
                #{block.id}
              </span>

              <span>
                {block.start} KB
              </span>

              <span>
                {block.size} KB
              </span>

              <span>
                {block.free ? (
                  <span className="block-free-badge">
                    Free
                  </span>
                ) : (
                  <span className="block-used-badge">
                    Used
                  </span>
                )}
              </span>

              <span className="block-process">
                {block.name}
              </span>
            </div>
          ))}

        </div>

      </div>

      <div className="allocation-history">

        <div className="card-header">
          <div>
            <h3>Allocation History</h3>
            <p>
              Recent memory management operations
            </p>
          </div>
        </div>

        {history.length === 0 ? (
          <div className="empty-history">
            No allocation activity yet.
          </div>
        ) : (
          <div className="history-list">

            {history.slice(0, 8).map(item => (
              <div
                className={`history-item ${
                  item.type === "allocate"
                    ? "history-allocate"
                    : "history-deallocate"
                }`}
                key={item.id}
              >
                <div>
                  <strong>
                    {item.type === "allocate"
                      ? "Allocation"
                      : "Deallocation"}
                  </strong>

                  <span>
                    {item.process}
                  </span>
                </div>

                <div>
                  <strong>
                    {item.size} KB
                  </strong>

                  <span>
                    {item.strategy}
                  </span>
                </div>

                <small>
                  {item.time}
                </small>
              </div>
            ))}

          </div>
        )}

      </div>

    </div>
  );
}

export default PhysicalAllocator;