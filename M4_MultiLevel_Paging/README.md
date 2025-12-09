# 🧠 Virtual Memory Simulator (Milestone 4)

This is the fourth version of my Operating System simulator. In this milestone, I implemented **Multi-Level Paging** — the approach used by real systems (like x86) to handle memory efficiently, rather than using one huge page table.

---

## 🚀 What I Built

### 1. Multi-Level Paging (The Tree Structure)

In the previous version (M3), I used one big array for the page table. That approach wasted a lot of memory.

In this version, I implemented a **2-Level Paging Hierarchy**:

- **Level 1 – Page Directory**
  - The main table
  - Each entry points to a second-level page table
  
- **Level 2 – Page Tables**
  - These contain the actual physical frame numbers
  
- **Address Splitting**
  - A 32-bit virtual address is split into:
    - `10 bits` → Directory Index
    - `10 bits` → Table Index
    - `12 bits` → Offset

This structure models real hardware designs used in modern CPUs.

---

### 2. Smart "Lazy" Allocation

The system starts **completely empty**. Nothing is allocated in advance.

- When a virtual address is accessed, a **Page Fault Handler** is triggered
- The handler:
  - Creates the required page table if it doesn't exist
  - Allocates a physical frame in RAM
- Only the needed parts of the page table tree are created

This significantly reduces memory waste and matches real OS behavior.

---

### 3. Physical Memory (RAM Simulation)

I simulate **128 KB of physical RAM**.

- RAM is represented by a global array (simulating real hardware)
- You can:
  - `Store(Address, Value)` → write data to memory
  - `Load(Address)` → read data back
- All memory accesses go through the full virtual → physical translation process

---

### 4. Visualizer Mode 🕵️‍♂️

To make the system easier to understand and debug, I added a **Visualizer Mode**.

When you enter a virtual address, the simulator prints the full "tree walk":

1. Page Directory entry lookup
2. Page Table entry lookup
3. Final Physical Address calculation

This makes the translation process visible step-by-step, instead of being a black box.

#### 📋 Example Scenario: Before and After Allocation

**Scenario 1: Inspecting an Unmapped Address**

When you inspect address `0x1000` before storing anything:

```
Enter Address to Inspect (Hex): 0x1000
   [VISUALIZER] Inspecting VA: 0x1000
   ├── 1. Directory Index: 0
   ├── [X] Directory Entry is NULL (Unmapped Region)
   └── [Result] Page Fault would occur here.
```

The system shows that:
- The Directory Index exists
- But the Page Table hasn't been created yet
- This address would trigger a **Page Fault** if accessed

**Scenario 2: Store Data at the Address**

Now store character `'A'` at address `0x1000`:

```
Enter Operation (W/R): W
Enter Address (Hex): 0x1000
Enter Value (Char): A
Stored 'A' at Physical Address 0x1f000
```

The Page Fault Handler automatically:
- Creates the missing Page Table
- Allocates a physical frame (Frame #31)
- Maps the virtual address to physical address `0x1f000`

**Scenario 3: Inspect the Same Address Again**

Now when you inspect `0x1000`:

```
Enter Address to Inspect (Hex): 0x1000
   [VISUALIZER] Inspecting VA: 0x1000
   ├── 1. Directory Index: 0
   ├── [OK] Page Table Found.
   ├── 2. Table Index: 1
   ├── [OK] Frame Found (Valid Bit = 1).
   └── 3. Physical Frame: 31 | Offset: 0
       -> Physical Address: 0x1f000
```

Now the full translation path exists:
- Directory Entry ✓ points to Page Table
- Page Table Entry ✓ points to Physical Frame #31
- Final physical address: `0x1f000`

This demonstrates **lazy allocation** in action — memory structures are only created when needed.

---

## 🛠️ How to Run It

### Compile

```bash
g++ -o os_sim MultiLevel_Paging.cpp
```

### Run

```bash
./os_sim
```

---

## 🎮 How to Use

The program supports **three modes**:

1. **Run Batch Test**
   - Reads commands from `input.txt`
   - Useful for automated testing

2. **Interactive Mode**
   - You type commands manually
   - Example:
     ```
     Store 100 55
     Load 100
     ```

3. **Visualizer Mode**
   - Enter a virtual address (e.g., `0xAABBCC`)
   - The simulator shows every translation step

---

## 📚 Specs

| Component            | Details                                               |
|----------------------|-------------------------------------------------------|
| **Page Size**        | 4 KB                                                  |
| **RAM Size**         | 128 KB (32 Frames)                                    |
| **Address Space**    | 32-bit                                                |
| **Paging Structure** | Directory (1024 entries) → Page Table (1024 entries) |

---

## 🏗️ My Progress

- **M1:** Simple identity mapping (Virtual = Physical)
- **M2:** Single large page table
- **M3:** Added RAM and Page Fault Handler
- **M4 (This Version):** Multi-Level Page Tables + Visualizer

---

> This project is built as a **learning simulator**, focusing on understanding how real operating systems manage memory internally.
