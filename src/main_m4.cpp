#include "paging.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <climits>

using namespace std;

// --- Physical Memory Configuration ---
const int PHY_MEM_SIZE = 64; 

struct Frame {
    int owner_vpn = -1; // Reverse Mapping: Which VPN owns this frame?
    bool is_free = true;
    int last_access_time = 0; // Architecture Optimization: Track time here for fast LRU
};

// --- Globals ---
PageDirectory* root_directory;
Frame ram[PHY_MEM_SIZE];
int global_clock = 0;

// --- Helper: Find and Evict the Least Recently Used Frame ---
int evict_lru() {
    int min_time = INT_MAX;
    int victim_frame = -1;

    // 1. Scan Physical RAM to find the oldest frame
    for (int i = 0; i < PHY_MEM_SIZE; i++) {
        if (!ram[i].is_free && ram[i].last_access_time < min_time) {
            min_time = ram[i].last_access_time;
            victim_frame = i;
        }
    }

    if (victim_frame == -1) {
        cerr << "Error: Memory is full but no pages to evict!" << endl;
        exit(1);
    }

    // 2. Invalidate the OLD owner (The Reverse Map)
    int old_vpn = ram[victim_frame].owner_vpn;
    int dir_idx = (old_vpn >> 10) & 0x3FF;  // Extract top 10 bits
    int tbl_idx = old_vpn & 0x3FF;          // Extract next 10 bits
    
    // We assume the page table exists because the frame was allocated
    if (root_directory->tables[dir_idx] != nullptr) {
        root_directory->tables[dir_idx]->entries[tbl_idx].valid = false;
        root_directory->tables[dir_idx]->entries[tbl_idx].frame_number = -1;
    }

    cout << "\033[1;33m  [EVICT] Frame " << victim_frame 
         << " was owning VPN " << old_vpn << " (Time: " << min_time << ")\033[0m" << endl;

    // 3. Return the now-empty frame
    return victim_frame;
}

// --- Helper: Allocate Frame (with Eviction) ---
int allocate_frame(int vpn) {
    // 1. Try to find a free frame
    for (int i = 0; i < PHY_MEM_SIZE; i++) {
        if (ram[i].is_free) {
            ram[i].is_free = false;
            ram[i].owner_vpn = vpn;
            ram[i].last_access_time = global_clock;
            return i;
        }
    }
    
    // 2. If FULL -> Perform LRU Eviction
    int victim = evict_lru();
    
    // 3. Re-assign the victim frame
    ram[victim].is_free = false;
    ram[victim].owner_vpn = vpn;
    ram[victim].last_access_time = global_clock;
    
    return victim;
}

// --- MMU: Translate Virtual Address to Physical Frame ---
int translate_address(uint32_t virtual_addr) {
    global_clock++; // Time ticks on every request

    // Breakdown
    int dir_index = (virtual_addr >> DIR_SHIFT) & 0x3FF;
    int table_index = (virtual_addr >> TABLE_SHIFT) & 0x3FF;
    int vpn = (virtual_addr >> 12);

    cout << "Time: " << setw(3) << global_clock << " | Req: 0x" << hex << virtual_addr << dec 
         << " (VPN: " << vpn << ") ... ";

    // 1. Check Directory
    if (root_directory->tables[dir_index] == nullptr) {
        root_directory->tables[dir_index] = new PageTable();
    }

    PageTable* pt = root_directory->tables[dir_index];

    // 2. Check Page Table (MISS)
    if (!pt->entries[table_index].valid) {
        cout << "\033[1;31mMISS\033[0m -> "; 

        int new_frame = allocate_frame(vpn);
        
        pt->entries[table_index].frame_number = new_frame;
        pt->entries[table_index].valid = true;
        
        cout << "Allocated Frame " << new_frame << endl;
        return new_frame;
    }

    // 3. HIT
    int frame = pt->entries[table_index].frame_number;
    
    // IMPORTANT: Update timestamp on Frame for LRU to work!
    ram[frame].last_access_time = global_clock;
    
    cout << "\033[1;32mHIT\033[0m  -> Frame " << frame << endl;
    return frame;
}

int main() {
    printf("=== Milestone 4: Multi-Level Paging + LRU ===\n");
    printf("RAM Size: %d Frames\n\n", PHY_MEM_SIZE);
    
    root_directory = new PageDirectory();

    // 1. Fill up memory (0 to 63)
    printf("--- Phase 1: Filling Memory ---\n");
    for(int i=0; i<64; i++) {
        // VPN i mapped to address i * 4096
        translate_address(i * 4096);
    }

    // 2. Access VPN 0 again to make it "Recent" (Time will update)
    // If LRU works, VPN 0 should NOT be evicted next. VPN 1 should be the victim.
    printf("\n--- Phase 2: Update VPN 0 Timestamp ---\n");
    translate_address(0x00000000); 

    // 3. Force Eviction (Access VPN 64)
    // Memory is full. Who gets kicked out? 
    // It should be VPN 1 (Time 2), because VPN 0 was just refreshed.
    printf("\n--- Phase 3: Force Eviction ---\n");
    translate_address(64 * 4096);

    return 0;
}
