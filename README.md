# OS Paging Simulator

A comprehensive C++ simulator modeled after the Memory Management Unit (MMU) of standard modern CPU architectures. This project explores and mimics the behavior of virtual-to-physical address translation through various historical and modern paging schemes.

## Project Overview

The OS Paging Simulator aims to unpack the "black box" of operating system memory virtualization. It supports a fully operational Translation Lookaside Buffer (TLB) paired with both Inverted Page Tables (IPT) and hierarchical Multi-Level Paging schemes. Furthermore, it manages simulated RAM pressure using an optimized Least-Recently-Used (LRU) algorithm.

## Architecture

The repository is branched into simulation milestones:

- **Milestone 3 (IPT & TLB)**: Simulates an associatively cached translation layer. If the TLB misses, it invokes a hash-based Inverted Page Table. It utilizes collision-chaining and strict TLB LRU replacement.
- **Milestone 4 (Multi-Level Paging)**: Simulates 32-bit `x86` hierarchical directory/table structures using explicit bit-shifting (10-bit / 10-bit / 12-bit). It also introduces "Reverse Mapped" physical frames to optimize Page Table Entry invalidations during deep memory evictions.

```text
      Virtual Address (32-bit)
   [10-bit Dir] [10-bit Tab] [12-bit Off]
         |            |            |
         v            |            |
  +---------------+   |            |
  | PageDirectory |   |            |
  +---------------+   |            |
         | (Pointer)  |            |
         v            v            |
    +---------+ +------------+     |
    |  Table  | | TableEntry |     |
    +---------+ +------------+     |
                      | (Frame #)  |
                      v            v
                +-----------------------+
                |     Physical RAM      |
                | (Reverse Map -> VPN)  |
                +-----------------------+
```

## Features

- **Hierarchical Address Translation**: Bitwise parsing of 32-bit virtual addresses into Physical Frames and Offsets.
- **Hardware Caching**: Simulates TLB Hit/Miss logic, improving subsequent lookup speeds dynamically.
- **LRU Page Replacement**: Accurately tracks temporal access (using a `Global_System_Clock`) to evict the oldest frames when simulated RAM thresholds are exceeded.
- **CLI Visualizer**: Real-time terminal output detailing hash-bucket chains, tree traversal, and Hit/Miss metrics.

## Technologies Used

- **Language**: C++17
- **Build System**: CMake

## Installation

Clone the repository and compile using CMake.

```bash
git clone https://github.com/Mido191020/paging-simulator.git
cd paging-simulator
cmake .
make
```

## Usage

The build process generates dual executables mirroring the project milestones:

### 1. Inverted Page Table + TLB Simulator

```bash
./paging_sim_m3
```

*Run this to access an interactive CLI menu allowing you to perform manual Store/Load operations or run automated batch scripts to witness TLB evictions.*

### 2. Multi-Level Hierarchical Paging Simulator

```bash
./paging_sim_m4
```

*Executes an automated, deterministic simulation demonstrating memory capacity saturation, chronological LRU victim selection, and automated Page Table unlinking.*
