# Paging Simulator (OS Memory Management)

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B-00599C.svg)
![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)

## 📖 Overview
A modular Simulation of an Operating System's Memory Management Unit (MMU). This project demonstrates how modern kernels handle virtual memory, specifically focusing on **Paging**, **Address Translation**, and **Page Replacement Algorithms**.

Currently active: **Milestone 3 (Least Recently Used - LRU)**.

## 🚀 Key Features
- **Virtual Memory Simulation:** Maps virtual page numbers (VPN) to physical frames.
- **Page Replacement Algorithms:**
  - **LRU (Least Recently Used):** Accurate tracking of page usage with timestamps.
  - *Future:* FIFO, Clock, Optimal.
- **Visualizer:** Real-time console output showing hits, misses, and memory state.
- **Trace Generation:** Simulates "Hot" and "Cold" memory access patterns.

## 🛠️ Build & Run

### Prerequisites
- C++ Compiler (GCC/Clang)
- CMake (3.10+)

### Quick Start
```bash
# 1. Clone and enter
git clone [https://github.com/Mido191020/paging-simulator.git](https://github.com/Mido191020/paging-simulator.git)
cd paging-simulator

# 2. Build
cmake -S . -B build
cmake --build build

# 3. Run
./build/paging_sim
📂 Project Structure
Plaintext

paging-simulator/
├── src/            # Source code (MMU logic, page tables)
├── docs/           # Technical documentation & architecture notes
├── milestones/     # Archive of previous milestones (M1-M6)
├── tests/          # Unit tests
└── input.txt       # Generated memory trace files
🧠 Architecture
The simulator mimics a demand-paging system:

CPU Request: Generates a Virtual Address (VPN).

MMU Lookup: Checks the Page Table.

Hit: Returns Frame Number instantly.

Miss:

Page Fault Handler wakes up.

Finds a free frame or selects a Victim (using LRU).

Updates mapping and resumes execution.

🤝 Contributing
Contributions are welcome! Please read CONTRIBUTING.md for details on our code of conduct and the process for submitting pull requests.

📜 License
This project is licensed under the MIT License - see the LICENSE file for details. EOF


### Step 2: Generate `CONTRIBUTING.md`
Professional repos always tell others how to contribute.

```bash
cat > CONTRIBUTING.md << 'EOF'
# Contributing to Paging Simulator

Thank you for your interest in contributing!

## How to Contribute
1. **Fork** the repository.
2. **Clone** your fork locally.
3. Create a **Branch** for your feature (`git checkout -b feature/AmazingFeature`).
4. **Commit** your changes.
5. **Push** to the branch.
6. Open a **Pull Request**.

## Coding Standards
- Use `src/` for all source code.
- Keep functions small and focused.
- Add comments for complex paging logic (especially around the LRU eviction).
