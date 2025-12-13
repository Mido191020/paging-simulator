# 📝 Inverted Page Table Simulator (Milestone 6)

## Overview
This project simulates an **Inverted Page Table (IPT)**, a fundamental data structure in modern operating systems for virtual memory management. Unlike traditional multi-level page tables, the IPT reduces memory usage by storing only the pages that actually exist in physical RAM.

The IPT is designed to answer the question:
> **“Given a Virtual Address (VA) and a Process ID (PID), what is the corresponding Physical Address (PA)?”**

It uses a **hash table with chaining** to efficiently handle collisions when multiple pages map to the same bucket.

## 🧠 Concepts Covered

### 1. Virtual Address (VA) and Virtual Page Number (VPN)
A virtual address is divided into:
- **VPN (upper bits):** Identifies the virtual page.
- **Offset (lower bits):** Identifies the location within the page.

To translate VA to PA, the VPN is needed for lookup, and the offset is added at the end.

### 2. Process ID (PID)
In a multi-process system, different processes can use the same VPN. The PID is combined with the VPN to uniquely identify a page globally.

### 3. Inverted Page Table Structure
- **Index:** Corresponds to the Hash Bucket.
- **Value:** Stores `(PID, VPN, PFN, pointer to next entry)`.
- **Buckets:** An array of linked lists to handle hash collisions (chaining).

### 4. Hashing
The key `(PID, VPN)` is passed through a hash function to determine which bucket to check. The PFN is stored inside the linked list node, not computed directly by the hash.

### 5. Chaining
Multiple entries that hash to the same bucket are stored in a linked list.
**Lookup Logic:**
1. Hash `(PID, VPN)` → bucket index.
2. Walk through the linked list.
3. Compare PID and VPN for each node.
   - If match → return **PFN** → compute **PA**.
   - If not found → **PAGE FAULT**.

## ⚙️ Translation Process (Human Version)
1. Receive **VA** and **PID**.
2. Extract **VPN** and **Offset** from VA.
3. Compute `Hash(PID, VPN)` → bucket index.
4. Access `bucket[index]`.
5. Walk linked list:
   - If PID & VPN match → found → read **PFN**.
   - If list ends → **PAGE FAULT**.
6. Compute **Physical Address (PA)**:
   ```cpp
   PA = (PFN * PAGE_SIZE) + offset;
