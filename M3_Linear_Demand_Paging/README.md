# Milestone 3: Linear Demand Paging with Physical Memory

## 📝 Overview
This module simulates a **Memory Management Unit (MMU)** using a **Single-Level (Linear) Page Table**. It implements a full **Demand Paging** system where physical frames are allocated dynamically only when a process accesses a virtual page (Lazy Allocation). It also simulates physical RAM to ensure data persistence via Store and Load operations.

## ✨ Key Features
* **Architecture:** Single-Level Page Table (Linear Array).
* **Demand Paging:** Handles Page Faults by intercepting invalid accesses and allocating frames on-the-fly.
* **Physical Memory Simulation:** * Simulates 128KB of byte-addressable RAM (`unsigned char RAM[]`).
    * Implements `Store(VA, Data)` and `Load(VA)` to read/write actual data.
* **Free Frame Management:** Uses a Stack-based Allocator (LIFO) to manage physical frames.
* **Protection:** Detects and handles Segmentation Faults (Out of Bounds).
* **Batch Processing:** Reads a trace of memory accesses from `input.txt` for automated testing.

## ⚙️ System Specifications
* **Page Size:** 4 KB (4096 bytes).
* **Physical Memory:** 128 KB (32 Frames).
* **Virtual Address Space:** Up to 1000 Pages (Simulated).
* **Addressing:** 32-bit logical addressing simulation.

## 🚀 How It Works
1.  **System Boot:** Initializes an empty Page Table (all pages invalid) and a full Free Frame List.
2.  **Translation:** * Checks if the page is valid.
    * If **Invalid (Page Fault)**: Calls the Handler -> Allocates Frame -> Updates Table -> Retries.
    * If **Valid**: Returns the Physical Frame Number (PFN).
3.  **Access:**
    * **Store:** Translates VA -> PA and writes data to the global RAM array.
    * **Load:** Translates VA -> PA and retrieves data from the global RAM array.

## 🧪 Test Case (Batch Input)
The system processes `input.txt` containing hex addresses. 
* **Scenario:** Accessing `0x1000` triggers a Page Fault (Allocation).
* **Scenario:** Writing 'A' to `0x1000` and reading it back proves memory persistence.
* **Scenario:** Accessing `0x999999` triggers a Segmentation Fault (Protection).
