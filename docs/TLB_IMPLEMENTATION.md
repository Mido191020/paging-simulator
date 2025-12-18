# TLB Implementation Guide

## Overview
The Translation Lookaside Buffer (TLB) is a hardware cache used to reduce the time taken to access a user memory location. It stores recent translations of virtual memory to physical memory.

## Data Structures
We model the TLB using a fixed-size array of entries.

```cpp
struct TLB_Entry {
    int vpn;            // Virtual Page Number (Key)
    int pfn;            // Physical Frame Number (Value)
    bool is_valid;      // Is this entry usable?
    int last_access;    // For LRU replacement inside the TLB
};
Algorithms
1. Lookup (Hit/Miss)
Input: Virtual Page Number (VPN)

Process: Iterate through TLB entries.

If entry.vpn == VPN and entry.is_valid: TLB HIT.

Else: TLB MISS.

2. Replacement Policy
When the TLB is full and a new translation must be added:

We use LRU (Least Recently Used).

Identify the entry with the smallest last_access timestamp and overwrite it.

Performance Impact
Without TLB: Every memory access requires 2 physical memory accesses (1 for Page Table + 1 for Data).

With TLB: 99% of accesses only require 1 physical memory access (TLB Hit). EOF


**Action 3: Create `docs/USAGE.md`**
A simple guide on how to run your simulator.

```bash
cat > docs/USAGE.md << 'EOF'
# Usage Guide

## Building the Project
```bash
cmake -S . -B build
cmake --build build
Running the Simulation
The executable expects an input.txt file in the same directory, or generates one if missing.

Bash

./build/paging_sim
Interpretation of Output
Green Text: Page Hit (Data found in memory).

Red Text: Page Miss (Page Fault occurred).

Yellow Text: Timestamp update (LRU tracking).

Modifying Parameters
To change memory size or trace length, modify src/main.cpp:

C++

const int Memory_size = 20;  // Change RAM size
