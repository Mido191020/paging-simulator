# 🧠 Virtual Memory Simulator (Milestone 3.5: N-Level Paging)

A generic, configurable implementation of a **Multi-Level Paging MMU**, simulating the modern **x86-64 (4-Level)** memory architecture. This project demonstrates how Operating Systems manage massive 64-bit address spaces using hierarchical structures and demand paging.

---

## 🚀 Key Features

### 1. 64-Bit Architecture Support
- Simulates the standard **48-bit Virtual Address Space** used in modern processors.
- Implements the **4-Level Paging Hierarchy**:
  1. **PML4** (Page Map Level 4)
  2. **PDPT** (Page Directory Pointer Table)
  3. **PD** (Page Directory)
  4. **PT** (Page Table)

### 2. Generic N-Level Design ⚙️
- **No Hardcoding:** The system is built on recursive-style logic that supports any number of levels (`LEVELS = N`).
- **Configuration:**
  ```cpp
  const int LEVELS = 4;
  const int SHIFT_ARR[LEVELS] = {39, 30, 21, 12}; // Dynamic Bit Shifting
  ```

### 3. Smart Data Structures
- Uses a single **Generic Page Table Struct** for all levels.
- Implements **Unions** to efficiently store pointers (for branches) or frame numbers (for leaves) in the same memory footprint.

### 4. Self-Healing Memory (Demand Paging)
- Memory is allocated **lazily**. Accessing a deep address (e.g., `0x1A00200300`) automatically constructs the required 4 layers of tables on the fly.

### 5. Deep Visualizer 🕵️‍♂️
A debugging tool that traces the hardware tree walk step-by-step:

```
[VISUALIZER 64-bit] Inspecting VA: 0x1A00200300
├── Level 4 (PML4) | Index: 0
│   └── [OK] Table Found -> Going deeper...
├── Level 3 (PDPT) | Index: 6
│   └── [OK] Table Found -> Going deeper...
├── Level 2 (PD)   | Index: 256
│   └── [OK] Table Found -> Going deeper...
├── Level 1 (PT)   | Index: 2
│   └── [OK] Leaf Found -> Frame Number: 31
└── [RESULT] Physical Address: 0x1f300
```

---

## 🛠️ Technical Implementation

### The Geometry
The simulator slices a virtual address into indexes based on the configuration:

| Level | Shift  | Mask   | Role                  |
|-------|--------|--------|-----------------------|
| L4    | >> 39  | 0x1FF  | Selects PML4 Entry    |
| L3    | >> 30  | 0x1FF  | Selects PDPT Entry    |
| L2    | >> 21  | 0x1FF  | Selects PD Entry      |
| L1    | >> 12  | 0x1FF  | Selects PT Entry      |
| Offset| N/A    | 0xFFF  | Selects Byte in Page  |

### The "Walker" Algorithm
Instead of hardcoding functions for each level, the system uses a loop:

1. Start at `Root_Table`.
2. For `i = 0` to `LEVELS`:
   - Calculate `Index`.
   - If invalid → Allocate new table (Branch) or Frame (Leaf).
   - Move pointer to `next_level_table`.
3. Return `Physical Address`.

---

## 🎮 How to Run

### Compile
```bash
g++ -o mmu_sim Generic_Paging_64bit.cpp
```

### Run
```bash
./mmu_sim
```

### Interactive Modes
The program offers a menu to test the system:

1. **Batch Mode**: Runs a script with huge 64-bit addresses.
2. **Interactive Mode**: Manually Store and Load data.
3. **Visualizer**: Type an address to see the tree structure without modifying it.

---

## 🔧 Configuration

Want to simulate a **5-level** paging system (like Intel's newer CPUs)?

```cpp
const int LEVELS = 5;
const int SHIFT_ARR[LEVELS] = {48, 39, 30, 21, 12};
```

The code automatically adapts! 🎯

---

## 📝 Example Output

```
[STORE] VA: 0x1A00200300 -> Data: 42
  ├── Allocated PML4 Entry 0
  ├── Allocated PDPT Entry 6
  ├── Allocated PD Entry 256
  ├── Allocated PT Entry 2 -> Frame 31
  └── Physical Address: 0x1f300

[LOAD] VA: 0x1A00200300
  └── Retrieved Data: 42 ✓
```

---

## 🏆 Project Highlights

✅ **Zero hardcoded levels** – fully generic design  
✅ **Realistic x86-64 simulation** – matches Intel/AMD behavior  
✅ **Educational visualizer** – see the MMU in action  
✅ **Extensible** – add TLB caching, page replacement, etc.

---

**Built with ❤️ by a Junior Systems Engineer.**
