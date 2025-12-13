# Advanced Paging Simulator

> A high-performance, educational simulation of Operating System memory management, featuring Multi-Level and Inverted Page Table implementations.

## 📖 Overview
This project simulates the hardware/software interface of an MMU (Memory Management Unit). Unlike simple allocators, this simulator handles complex virtual memory challenges including **hierarchical translation**, **demand paging**, and **page replacement policies**.

It is designed to demonstrate deep systems knowledge:
* **Virtualization:** Decoupling logical addresses from physical RAM.
* **Optimization:** Comparing Linear vs. Multi-Level vs. Inverted tables.
* **Algorithm Design:** Handling page faults and memory constraints efficiently.

---

## 🏗️ Repository Structure

| Directory | Content |
| :--- | :--- |
| `milestones/` | Core implementations separated by complexity (M3 - M6). |
| `src/` | Shared entry points and driver code. |
| `inputs/` | Memory trace files used for simulation benchmarks. |
| `docs/` | Technical documentation and architectural decisions. |

---

## 🚀 Key Implementations

### 1. Linear Demand Paging (M3)
The foundational implementation using a direct array map. Simulates basic validity checks and physical frame allocation.

### 2. Multi-Level Paging (M4 & M5)
Implements a Page Directory structure to handle sparse address spaces. M5 generalizes this to **N-levels**, allowing dynamic configuration of the page table depth.

### 3. Inverted Page Table (M6)
A space-efficient approach that scales with physical memory rather than virtual address space, using hashing to map frames.

---

## 🛠️ Usage

To run a specific milestone simulation (e.g., M4):

```bash
cd milestones/M4_MultiLevel_Paging
# Ensure your CMake or Compile command points to the input correctly
./main ../../inputs/input.txt
```

---

## 📚 Documentation
* [System Architecture](docs/architecture.md)
* [Memory Theory & Concepts](docs/memory_concepts.md)
* [Development Roadmap](docs/milestones.md)

---
**License:** MIT
