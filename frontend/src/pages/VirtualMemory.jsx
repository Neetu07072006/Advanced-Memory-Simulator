import { useState } from "react";
import { useSimulator } from "../context/SimulatorContext";

const initialTLB = [
  { page: 0, frame: 2 },
  { page: 1, frame: 0 },
  { page: 3, frame: 1 },
  { page: 4, frame: 3 }
];

const initialPageTable = [
  { page: 0, frame: 2, valid: true },
  { page: 1, frame: 0, valid: true },
  { page: 2, frame: 1, valid: true },
  { page: 3, frame: null, valid: false },
  { page: 4, frame: 3, valid: true },
  { page: 5, frame: null, valid: false }
];

function VirtualMemory() {
  const {
    recordTranslation,
    addActivity
  } = useSimulator();

  const [algorithm, setAlgorithm] = useState("FIFO");
  const [pageSize, setPageSize] = useState(4);
  const [tlbCapacity, setTlbCapacity] = useState(4);
  const [address, setAddress] = useState("18");

  const [tlb, setTlb] = useState(initialTLB);
  const [pageTable, setPageTable] = useState(initialPageTable);

  const [translation, setTranslation] = useState(null);
  const [history, setHistory] = useState([]);

  const [message, setMessage] = useState({
    type: "info",
    title: "Ready for translation",
    text: "Enter a logical address and translate it."
  });

  const resetSimulation = () => {
    setTlb(initialTLB);
    setPageTable(initialPageTable);
    setTranslation(null);
    setHistory([]);

    setMessage({
      type: "info",
      title: "Simulation reset",
      text: "Virtual memory state has been restored."
    });
  };

  const addToTLB = (page, frame) => {
    setTlb(prev => {
      const filtered = prev.filter(item => item.page !== page);

      return [
        { page, frame },
        ...filtered
      ].slice(0, Number(tlbCapacity));
    });
  };

  const translateAddress = () => {
    const logicalAddress = Number(address);

    if (
      address === "" ||
      Number.isNaN(logicalAddress) ||
      logicalAddress < 0
    ) {
      setTranslation(null);

      setMessage({
        type: "error",
        title: "Invalid address",
        text: "Enter a valid non-negative logical address."
      });

      return;
    }

    const size = Number(pageSize);

    const page = Math.floor(logicalAddress / size);
    const offset = logicalAddress % size;

    const tlbEntry = tlb.find(item => item.page === page);

    if (tlbEntry) {
      const physicalAddress =
        tlbEntry.frame * size + offset;

      const result = {
        logicalAddress,
        page,
        offset,
        frame: tlbEntry.frame,
        physicalAddress,
        tlbHit: true,
        pageFault: false
      };

      setTranslation(result);

      setHistory(prev => [
        result,
        ...prev
      ].slice(0, 8));

      setMessage({
        type: "success",
        title: "TLB Hit",
        text: `Page ${page} was found directly in the TLB.`
      });

      recordTranslation({
        hit: true,
        pageFault: false
      });

      addActivity(
        `TLB hit for page ${page}`,
        "success"
      );

      return;
    }

    const tableEntry = pageTable.find(
      item => item.page === page
    );

    if (!tableEntry) {
      setTranslation({
        logicalAddress,
        page,
        offset,
        frame: null,
        physicalAddress: null,
        tlbHit: false,
        pageFault: true
      });

      setMessage({
        type: "error",
        title: "Page Fault",
        text: `Page ${page} does not exist in the current page table.`
      });

      recordTranslation({
        hit: false,
        pageFault: true
      });

      addActivity(
        `Page fault for page ${page}`,
        "error"
      );

      return;
    }

    if (!tableEntry.valid) {
      setTranslation({
        logicalAddress,
        page,
        offset,
        frame: null,
        physicalAddress: null,
        tlbHit: false,
        pageFault: true
      });

      setMessage({
        type: "error",
        title: "Page Fault",
        text: `Page ${page} is currently not resident in physical memory.`
      });

      recordTranslation({
        hit: false,
        pageFault: true
      });

      addActivity(
        `Page fault for page ${page}`,
        "error"
      );

      return;
    }

    const physicalAddress =
      tableEntry.frame * size + offset;

    const result = {
      logicalAddress,
      page,
      offset,
      frame: tableEntry.frame,
      physicalAddress,
      tlbHit: false,
      pageFault: false
    };

    setTranslation(result);

    setHistory(prev => [
      result,
      ...prev
    ].slice(0, 8));

    addToTLB(
      page,
      tableEntry.frame
    );

    setMessage({
      type: "info",
      title: "TLB Miss",
      text: `Page ${page} was found in the page table and added to the TLB.`
    });

    recordTranslation({
      hit: false,
      pageFault: false
    });

    addActivity(
      `TLB miss for page ${page}`,
      "info"
    );
  };

  const validPages = pageTable.filter(
    item => item.valid
  ).length;

  const invalidPages = pageTable.filter(
    item => !item.valid
  ).length;

  const hitCount = history.filter(
    item => item.tlbHit
  ).length;

  const faultCount = history.filter(
    item => item.pageFault
  ).length;

  const hitRate =
    history.length > 0
      ? Math.round((hitCount / history.length) * 100)
      : 0;

  return (
    <div className="page">

      <div className="page-heading">
        <div>
          <span className="eyebrow">
            VIRTUAL MEMORY
          </span>

          <h2>
            Virtual Memory & TLB
          </h2>

          <p>
            Simulate logical address translation,
            page-table lookup, TLB caching and page faults.
          </p>
        </div>

        <div className="vm-algorithm">
          <span>Replacement</span>
          <strong>{algorithm}</strong>
        </div>
      </div>

      <div className="vm-controls">

        <div className="vm-control">
          <label>Replacement Algorithm</label>

          <select
            value={algorithm}
            onChange={e => setAlgorithm(e.target.value)}
          >
            <option value="FIFO">FIFO</option>
            <option value="LRU">LRU</option>
            <option value="OPTIMAL">Optimal</option>
          </select>
        </div>

        <div className="vm-control">
          <label>Page Size</label>

          <select
            value={pageSize}
            onChange={e => setPageSize(Number(e.target.value))}
          >
            <option value={2}>2 KB</option>
            <option value={4}>4 KB</option>
            <option value={8}>8 KB</option>
          </select>
        </div>

        <div className="vm-control address-control">
          <label>Logical Address</label>

          <input
            value={address}
            onChange={e => setAddress(e.target.value)}
            onKeyDown={e => {
              if (e.key === "Enter") {
                translateAddress();
              }
            }}
            placeholder="Enter address"
          />
        </div>

        <button
          className="primary-button"
          onClick={translateAddress}
        >
          Translate
        </button>

        <button
          className="secondary-button"
          onClick={resetSimulation}
        >
          Reset
        </button>

      </div>

      <div
        className={`translation-message ${
          message.type
        }`}
      >
        <strong>{message.title}</strong>
        <span>{message.text}</span>
      </div>

      <div className="vm-stat-grid">

        <div className="vm-stat">
          <span>Page Size</span>
          <strong>{pageSize} KB</strong>
          <small>Current configuration</small>
        </div>

        <div className="vm-stat">
          <span>TLB Capacity</span>
          <strong>{tlbCapacity}</strong>
          <small>Maximum entries</small>
        </div>

        <div className="vm-stat">
          <span>Valid Pages</span>
          <strong>{validPages}</strong>
          <small>Resident pages</small>
        </div>

        <div className="vm-stat">
          <span>Page Faults</span>
          <strong>{faultCount}</strong>
          <small>Current history</small>
        </div>

        <div className="vm-stat">
          <span>TLB Hit Rate</span>
          <strong>{hitRate}%</strong>
          <small>Translation history</small>
        </div>

      </div>

      <div className="vm-main-grid">

        <div className="page-table-card">

          <div className="card-header">
            <div>
              <h3>Page Table</h3>
              <p>Current virtual-to-physical mappings</p>
            </div>
          </div>

          <div className="page-table-head">
            <span>Page</span>
            <span>Frame</span>
            <span>Status</span>
          </div>

          {pageTable.map(entry => (
            <div
              className="page-table-row"
              key={entry.page}
            >
              <div className="page-cell">
                P{entry.page}
              </div>

              <div className="page-cell">
                {entry.valid
                  ? `F${entry.frame}`
                  : "—"}
              </div>

              <div className="page-cell">
                {entry.valid ? (
                  <span className="valid-badge">
                    Valid
                  </span>
                ) : (
                  <span className="invalid-badge">
                    Invalid
                  </span>
                )}
              </div>
            </div>
          ))}

        </div>

        <div className="tlb-card">

          <div className="card-header">
            <div>
              <h3>TLB</h3>
              <p>
                Most recently cached translations
              </p>
            </div>

            <select
              value={tlbCapacity}
              onChange={e =>
                setTlbCapacity(Number(e.target.value))
              }
            >
              <option value={2}>2</option>
              <option value={4}>4</option>
              <option value={6}>6</option>
              <option value={8}>8</option>
            </select>
          </div>

          <div className="tlb-list">

            {tlb.length === 0 ? (
              <div className="empty-tlb">
                No TLB entries
              </div>
            ) : (
              tlb.map((entry, index) => (
                <div
                  className="tlb-entry"
                  key={`${entry.page}-${index}`}
                >
                  <div className="tlb-index">
                    {index + 1}
                  </div>

                  <div>
                    <strong>
                      Page {entry.page}
                    </strong>

                    <span>
                      Frame {entry.frame}
                    </span>
                  </div>

                  <div className="mapped-status">
                    Mapped
                  </div>
                </div>
              ))
            )}

          </div>

        </div>

      </div>

      <div className="address-flow-card">

        <div className="card-header">
          <div>
            <h3>Address Translation Flow</h3>
            <p>
              Logical address decomposition and translation
            </p>
          </div>
        </div>

        <div className="translation-flow">

          <div className="flow-node">
            <span>Logical Address</span>
            <strong>
              {translation
                ? translation.logicalAddress
                : "—"}
            </strong>
          </div>

          <div className="flow-arrow">
            →
          </div>

          <div className="flow-node">
            <span>Page Number</span>
            <strong>
              {translation
                ? translation.page
                : "—"}
            </strong>
          </div>

          <div className="flow-arrow">
            →
          </div>

          <div className="flow-node">
            <span>Offset</span>
            <strong>
              {translation
                ? translation.offset
                : "—"}
            </strong>
          </div>

          <div className="flow-arrow">
            →
          </div>

          <div className="flow-node">
            <span>Frame</span>
            <strong>
              {translation &&
              translation.frame !== null
                ? translation.frame
                : "—"}
            </strong>
          </div>

          <div className="flow-arrow">
            →
          </div>

          <div className="flow-node">
            <span>Physical Address</span>
            <strong>
              {translation &&
              translation.physicalAddress !== null
                ? translation.physicalAddress
                : "—"}
            </strong>
          </div>

        </div>

      </div>

      <div className="vm-bottom-grid">

        <div className="vm-info-card">

          <div className="card-header">
            <div>
              <h3>Translation Result</h3>
              <p>Latest address translation</p>
            </div>
          </div>

          {translation ? (
            <div className="config-list">

              <div>
                <span>Logical Address</span>
                <strong>
                  {translation.logicalAddress}
                </strong>
              </div>

              <div>
                <span>Page Number</span>
                <strong>
                  {translation.page}
                </strong>
              </div>

              <div>
                <span>Offset</span>
                <strong>
                  {translation.offset}
                </strong>
              </div>

              <div>
                <span>Frame</span>
                <strong>
                  {translation.frame !== null
                    ? translation.frame
                    : "Page Fault"}
                </strong>
              </div>

              <div>
                <span>Translation Type</span>
                <strong>
                  {translation.pageFault
                    ? "Page Fault"
                    : translation.tlbHit
                      ? "TLB Hit"
                      : "TLB Miss"}
                </strong>
              </div>

            </div>
          ) : (
            <div className="empty-tlb">
              Translate an address to see the result.
            </div>
          )}

        </div>

        <div className="vm-info-card">

          <div className="card-header">
            <div>
              <h3>Translation History</h3>
              <p>Recent virtual memory activity</p>
            </div>
          </div>

          {history.length === 0 ? (
            <div className="empty-tlb">
              No translations yet.
            </div>
          ) : (
            <div className="config-list">

              {history.map((item, index) => (
                <div key={index}>
                  <span>
                    Address {item.logicalAddress}
                  </span>

                  <strong>
                    {item.pageFault
                      ? "Page Fault"
                      : item.tlbHit
                        ? "TLB Hit"
                        : "TLB Miss"}
                  </strong>
                </div>
              ))}

            </div>
          )}

        </div>

      </div>

      <div className="vm-legend">

        <div>
          <span className="legend-dot valid"></span>
          Valid Page
        </div>

        <div>
          <span className="legend-dot invalid"></span>
          Invalid Page
        </div>

        <div>
          <span className="legend-dot tlb"></span>
          TLB Entry
        </div>

        <div>
          <span className="legend-dot fault"></span>
          Page Fault
        </div>

      </div>

    </div>
  );
}

export default VirtualMemory;