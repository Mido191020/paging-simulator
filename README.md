# Paging Simulator

A minimal C++ simulator of an Operating System's Memory Management Unit (MMU).
It demonstrates virtual-to-physical address translation using an **Inverted Page Table (IPT)** backed by a hash table, a software **TLB** with LRU eviction, and demand-paging page faults.

## Features

- **Virtual memory simulation** — maps virtual page numbers (VPN) to physical frame numbers (PFN).
- **Inverted Page Table** — one entry per physical frame, hashed by `(PID, VPN)`.
- **TLB (fast path)** — small fully-associative cache with LRU replacement on miss.
- **Page fault handler** — allocates a free frame on miss; reports `Out of RAM` when full.
- **Visualizer** — prints TLB state, hash bucket lookup, and hit/miss per translation.
- **Batch & interactive modes** — run a built-in trace or enter `W/R` commands manually.

## Architecture

```text
                 CPU Request
                      │
                      ▼
              ┌───────────────┐
              │    TLB        │◄──── fast path (cached translations)
              │  (LRU cache)  │
              └───────────────┘
                      │ miss
                      ▼
              ┌───────────────┐
              │ Inverted Page │
              │   Table (IPT) │◄── slow path: hash(PID, VPN)
              └───────────────┘
                      │ miss (page fault)
                      ▼
              ┌───────────────┐
              │  Frame Alloc  │◄── allocate free frame or LRU evict
              └───────────────┘
                      │
                      ▼
              Physical Address = (PFN << 12) | offset
```

## Build & Run

```bash
# configure + build
cmake -S . -B build
cmake --build build

# run
./build/paging_sim
```

Requires a C++17 compiler and CMake 3.10+.

## Project Structure

```
paging-simulator/
├── main.cpp        # single source: IPT + TLB + LRU simulator
├── CMakeLists.txt  # build configuration
├── README.md       # this file
├── .gitignore      # ignores build/IDE artifacts
└── examples/
    └── input.txt   # sample memory-trace input
```

## Usage

On start you get a menu:

```
1. Run TLB/LRU Batch Test   - runs a built-in trace and prints hit/miss stats
2. Interactive Mode         - enter Op (W/R), PID, VA(hex)
3. Visualize Translation    - show TLB/IPT lookup steps for one address
0. Exit
```

Interactive example:

```
Enter Op (W/R), PID, VA(Hex): W 1 0x1000
Value: A
[RAM] PID 1 Stored 'A' at PA 0x1000
```

## License

MIT
