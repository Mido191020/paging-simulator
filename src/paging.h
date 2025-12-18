#ifndef PAGING_H
#define PAGING_H

#include <vector>
#include <iostream>

// --- Constants for 32-bit Architecture ---
// Virtual Address: | Directory (10) | Table (10) | Offset (12) |
const int PAGE_SIZE = 4096;
const int DIR_SHIFT = 22;   // Bits 22-31
const int TABLE_SHIFT = 12; // Bits 12-21

// --- Structures ---

// Level 2: Page Table Entry (The actual mapping)
struct PageTableEntry {
    int frame_number = -1;
    bool valid = false;
    int last_access_time = 0; // For LRU
};

// Level 2: A single Page Table (Contains 1024 entries)
struct PageTable {
    PageTableEntry entries[1024];
};

// Level 1: Page Directory (Contains pointers to Page Tables)
struct PageDirectory {
    // Pointers to Level 2 tables (null if not allocated)
    PageTable* tables[1024]; 
    
    PageDirectory() {
        for(int i=0; i<1024; i++) tables[i] = nullptr;
    }
};

#endif
