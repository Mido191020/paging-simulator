
# 📄 Paging Simulator (OS Memory Management)

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B-00599C.svg)
![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)

## 📖 Overview

A **modular simulation** of an Operating System’s **Memory Management Unit (MMU)**.  
This project demonstrates how modern kernels handle **virtual memory**, with a focus on:

- Paging
- Address Translation
- Page Replacement Algorithms

> **Current milestone:** **M3 – Least Recently Used (LRU)**

---

## 🚀 Key Features

- **Virtual Memory Simulation**  
  Maps Virtual Page Numbers (VPNs) to Physical Frames.

- **Page Replacement Algorithms**
  - ✅ **LRU (Least Recently Used)** using timestamp-based tracking
  - 🔜 FIFO, Clock, Optimal

- **Console Visualizer**  
  Real-time output showing:
  - Page hits
  - Page misses
  - Memory frame state

- **Trace Generation**  
  Simulates realistic memory access patterns:
  - Hot pages
  - Cold pages

---

## 🛠️ Build & Run

### Prerequisites

- C++ Compiler (GCC or Clang)
- CMake **3.10+**

### Quick Start


# Clone the repository
git clone https://github.com/Mido191020/paging-simulator.git
cd paging-simulator

# Build the project
cmake -S . -B build
cmake --build build

# Run the simulator
./build/paging_sim



## 📂 Project Structure

```text
paging-simulator/
├── src/            # Core simulator logic (MMU, page tables, replacement)
├── docs/           # Technical documentation & architecture notes
├── milestones/     # Archived milestones (M1–M6)
├── tests/          # Unit tests
└── input.txt       # Generated memory access traces
```

---

## 🧠 Architecture

The simulator models a **demand-paging system**:

1. **CPU Request**
   Generates a virtual address (VPN).

2. **MMU Lookup**
   Checks the page table.

3. **Hit**
   Frame number is returned immediately.

4. **Miss (Page Fault)**

   * Page fault handler is invoked
   * Free frame is used or a victim page is selected (LRU)
   * Page table is updated
   * Execution resumes

---

## 🤝 Contributing

Contributions are welcome!
Please read **`CONTRIBUTING.md`** for coding standards and pull request guidelines.

---

