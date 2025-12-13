#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <cstdint>
#include <iomanip> // For nice formatting

using namespace std;

// Standardize Types
typedef uint64_t u64;

/* ===================================================
   SECTION 1: System Configuration
   =================================================== */
const u64 pageSize = 4096;
const u64 MEM_SIZE = 131072; // 128 KB
const u64 TOTAL_FRAMES = (MEM_SIZE / pageSize); // 32 Frames
const int RAM_SIZE = MEM_SIZE;

// HASH TABLE CONFIG
const int TABLE_SIZE = 10; // Small size to force collisions

// TLB CONFIG
const int TLB_TABLE_SIZE = 4; // Small size to force LRU eviction

// Error Codes
const u64 ERR_PAGE_FAULT = -1;

// GLOBAL CLOCK (For LRU)
u64 Global_System_Clock = 0;

/* ===================================================
   SECTION 2: Physical Memory (Hardware)
   =================================================== */
unsigned char RAM[RAM_SIZE];

typedef struct {
    int fram[TOTAL_FRAMES];
    int top;
} freeList;

freeList physical_memory;

void init_free_list(freeList* list) {
    list->top = -1;
    for (int i = 0; i < TOTAL_FRAMES; ++i) {
        list->top++;
        list->fram[list->top] = i;
    }
}

long long allocate_frame(freeList* list) {
    if (list->top == -1) return -1;
    int allocated_frame = list->fram[list->top];
    list->top--;
    return allocated_frame;
}

/* ===================================================
   SECTION 3: Inverted Page Table (The Slow Path)
   =================================================== */
struct Node {
    u64 PID;
    u64 VPN;
    u64 PFN;
    Node* next;
};

struct HashTable {
    Node* buckets[TABLE_SIZE];
};

HashTable* System_IPT = new HashTable;

void Init_IPT() {
    for(int i=0; i<TABLE_SIZE; i++) {
        System_IPT->buckets[i] = NULL;
    }
}

// --- Helpers ---
u64 get_VPN(u64 VA) { return VA >> 12; }
u64 get_offset(u64 VA) { return VA & 0xFFF; }
u64 construct_PA(u64 frame, u64 offset) { return (frame << 12) | offset; }

u64 Hash_Function(u64 PID, u64 VPN) {
    u64 combined = PID ^ VPN;
    return combined % TABLE_SIZE;
}

// --- IPT Logic ---
Node* find_node(Node* head, u64 PID, u64 VPN) {
    Node* current = head;
    while (current != NULL) {
        if (current->PID == PID && current->VPN == VPN) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void insert_node(u64 PID, u64 VPN, u64 PFN) {
    u64 idx = Hash_Function(PID, VPN);

    // Check if update existing
    Node* existing = find_node(System_IPT->buckets[idx], PID, VPN);
    if (existing) {
        existing->PFN = PFN;
        return;
    }

    // Insert New (Head Insertion)
    Node* new_node = new Node;
    new_node->PID = PID;
    new_node->VPN = VPN;
    new_node->PFN = PFN;
    new_node->next = System_IPT->buckets[idx];
    System_IPT->buckets[idx] = new_node;
}

// The "Heavy" Translator
u64 Translate_Inverted(u64 PID, u64 VA) {
    u64 vpn = get_VPN(VA);
    u64 offset = get_offset(VA);
    u64 idx = Hash_Function(PID, vpn);

    // 1. Lookup
    Node* target = find_node(System_IPT->buckets[idx], PID, vpn);

    u64 pfn;
    if (target != NULL) {
        pfn = target->PFN;
    } else {
        // Page Fault -> Allocate Frame
        long long new_frame = allocate_frame(&physical_memory);
        if (new_frame == -1) {
            cout << "CRITICAL ERROR: Out of RAM!\n";
            return ERR_PAGE_FAULT;
        }
        insert_node(PID, vpn, new_frame);
        pfn = new_frame;
    }

    return construct_PA(pfn, offset);
}

/* ===================================================
   SECTION 4: TLB Simulator (The Fast Path) ⚡
   =================================================== */

struct TLBEntry {
    u64 PID;
    u64 VPN;
    u64 PFN;
    bool is_vaild;
    u64 Timestampe; // For LRU
};

struct TLB_table {
    TLBEntry array[TLB_TABLE_SIZE];
};

TLB_table* System_TLB = new TLB_table;

// 1. Lookup (Reader)
long long TLB_Lookup(u64 PID, u64 VA) {
    u64 VPN = get_VPN(VA);

    for (int i = 0; i < TLB_TABLE_SIZE; ++i) {
        // Check for Valid Match
        if (System_TLB->array[i].is_vaild &&
            System_TLB->array[i].PID == PID &&
            System_TLB->array[i].VPN == VPN) {

            // HIT! Update LRU Time
            Global_System_Clock++;
            System_TLB->array[i].Timestampe = Global_System_Clock;
            return System_TLB->array[i].PFN;
        }
    }
    // Return -1 AFTER checking everyone
    return -1;
}

// 2. Update (Writer + LRU Eviction)
void TLB_Update(u64 PID, u64 VPN, u64 PFN) {
    // A. Try to find empty spot
    for (int i = 0; i < TLB_TABLE_SIZE; ++i) {
        if (System_TLB->array[i].is_vaild == false) {
            System_TLB->array[i].PID = PID;
            System_TLB->array[i].VPN = VPN;
            System_TLB->array[i].PFN = PFN;
            System_TLB->array[i].is_vaild = true;
            System_TLB->array[i].Timestampe = Global_System_Clock;
            return;
        }
    }

    // B. If full, Find Victim (Oldest Timestamp)
    int victim_index = 0;
    u64 min_time = System_TLB->array[0].Timestampe;

    for (int i = 1; i < TLB_TABLE_SIZE; ++i) {
        if (System_TLB->array[i].Timestampe < min_time) {
            min_time = System_TLB->array[i].Timestampe;
            victim_index = i;
        }
    }

    // C. Overwrite Victim
    System_TLB->array[victim_index].PID = PID;
    System_TLB->array[victim_index].VPN = VPN;
    System_TLB->array[victim_index].PFN = PFN;
    System_TLB->array[victim_index].is_vaild = true;
    System_TLB->array[victim_index].Timestampe = Global_System_Clock;
}

/* ===================================================
   SECTION 5: The Translation Manager 🚦
   =================================================== */
int TLB_Hits = 0;
int TLB_Misses = 0;

u64 Translate_With_TLB(u64 PID, u64 VA) {
    u64 VPN = get_VPN(VA);
    u64 offset = get_offset(VA);

    // Step 1: Try Fast Path
    long long tlb_pfn = TLB_Lookup(PID, VA);

    if (tlb_pfn != -1) {
        TLB_Hits++;
        return (tlb_pfn << 12) | offset;
    }

    // Step 2: Slow Path (Miss)
    TLB_Misses++;
    u64 PA = Translate_Inverted(PID, VA);

    if (PA == ERR_PAGE_FAULT) return ERR_PAGE_FAULT;

    // Step 3: Update Cache
    u64 new_PFN = PA >> 12;
    TLB_Update(PID, VPN, new_PFN);

    return PA;
}

/* ===================================================
   SECTION 6: Visualization Tools 🕵️‍♂️
   =================================================== */

void Print_TLB_State() {
    cout << "\n   [DEBUG] TLB State (Current Time: " << Global_System_Clock << ")\n";
    cout << "   --------------------------------------------------------------\n";
    for(int i=0; i<TLB_TABLE_SIZE; ++i) {
        cout << "   Slot " << i << ": ";
        if(System_TLB->array[i].is_vaild) {
            cout << "PID:" << System_TLB->array[i].PID
                 << " | VPN:" << System_TLB->array[i].VPN
                 << " | PFN:" << System_TLB->array[i].PFN
                 << " | Time:" << System_TLB->array[i].Timestampe << "\n";
        } else {
            cout << "[EMPTY]\n";
        }
    }
    cout << "   --------------------------------------------------------------\n";
}

void Visualize_Translation(u64 PID, u64 VA) {
    cout << "\n   [VISUALIZER] Inspecting PID: " << PID << " VA: 0x" << hex << VA << dec << "\n";

    // Check TLB
    long long tlb_pfn = TLB_Lookup(PID, VA); // Note: This will update timestamp if hit!

    if (tlb_pfn != -1) {
        cout << "   └── TLB: HIT! 🎯 -> Frame " << tlb_pfn << "\n";
    } else {
        cout << "   └── TLB: MISS ❌ -> Searching Inverted Page Table...\n";

        // Show Hashing
        u64 vpn = get_VPN(VA);
        u64 idx = Hash_Function(PID, vpn);
        Node* current = System_IPT->buckets[idx];

        cout << "       └── Hashing: (" << PID << "^" << vpn << ") % 10 = Bucket " << idx << "\n";
        bool found = false;
        while(current) {
            cout << "           └── Checking Node [PID:" << current->PID << ", VPN:" << current->VPN << "]... ";
            if(current->PID == PID && current->VPN == vpn) {
                cout << "MATCH! (Frame " << current->PFN << ")\n";
                found = true;
                break;
            }
            cout << "No.\n";
            current = current->next;
        }
        if(!found) cout << "           └── Not found (Page Fault will trigger).\n";
    }
}

/* ===================================================
   SECTION 7: User Interface (Store/Load)
   =================================================== */

void Store(u64 PID, u64 VA, char data) {
    u64 PA = Translate_With_TLB(PID, VA);
    if (PA != ERR_PAGE_FAULT) {
        RAM[PA] = data;
        cout << "   [RAM] PID " << PID << " Stored '" << data << "' at PA 0x" << hex << PA << dec << "\n";
    }
}

char Load(u64 PID, u64 VA) {
    u64 PA = Translate_With_TLB(PID, VA);
    if (PA != ERR_PAGE_FAULT) {
        cout << "   [RAM] PID " << PID << " Loaded '" << RAM[PA] << "' from PA 0x" << hex << PA << dec << "\n";
        return RAM[PA];
    }
    return '?';
}

u64 hex_to_int(string hex) {
    return stoull(hex, nullptr, 16);
}

void System_Boot() {
    init_free_list(&physical_memory);
    Init_IPT();

    // Clean TLB Registers
    for (int i = 0; i < TLB_TABLE_SIZE; ++i) {
        System_TLB->array[i].is_vaild = false;
        System_TLB->array[i].Timestampe = 0;
    }

    cout << "System Booted. Inverted Page Table + TLB Ready.\n";
}

void run_batch_test() {
    cout << "\n=== RUNNING TLB BATCH TEST ===\n";

    // Scenario:
    // 1. Fill TLB (4 entries)
    // 2. Access 5th entry (Force Eviction)
    // 3. Access 1st entry again (Should be Miss if evicted)

    ofstream out("input_tlb.txt");
    out << "W 1 0x1000 A\n" // Page 1
           "W 1 0x2000 B\n" // Page 2
           "W 1 0x3000 C\n" // Page 3
           "W 1 0x4000 D\n" // Page 4 (TLB Full now)
           "V 1 0x1000\n"   // Visualize Page 1 (Should be Hit)
           "W 1 0x5000 E\n" // Page 5 -> Should evict LRU
           "V 1 0x5000\n"   // Visualize Page 5
           "R 1 0x2000\n";  // Read Page 2
    out.close();

    ifstream inputFile("input_tlb.txt");
    if (!inputFile.is_open()) return;

    string hexAddr;
    char op, data;
    u64 pid;

    while (inputFile >> op >> pid >> hexAddr) {
        u64 VA = hex_to_int(hexAddr);
        cout << "\nCMD: " << op << " PID:" << pid << " VA:" << hexAddr << "\n";

        if (op == 'W') {
            inputFile >> data;
            Store(pid, VA, data);
        } else if (op == 'R') {
            Load(pid, VA);
        } else if (op == 'V') {
            Visualize_Translation(pid, VA);
            Print_TLB_State();
        }
    }
    inputFile.close();

    cout << "\n=== STATS ===\n";
    cout << "TLB Hits: " << TLB_Hits << "\n";
    cout << "TLB Misses: " << TLB_Misses << "\n";
}

int main() {
    System_Boot();

    int choice;
    do {
        cout << "\n========================================\n";
        cout << "   Memory Simulator (IPT + TLB + LRU)   \n";
        cout << "========================================\n";
        cout << "1. Run TLB/LRU Batch Test\n";
        cout << "2. Interactive Mode\n";
        cout << "3. Visualize Translation\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            run_batch_test();
        }
        else if (choice == 2) {
            u64 pid, va;
            string hexVA;
            char op, val;
            cout << "Enter Op (W/R), PID, VA(Hex): ";
            cin >> op >> pid >> hexVA;
            va = hex_to_int(hexVA);

            if (op == 'W' || op == 'w') {
                cout << "Value: "; cin >> val;
                Store(pid, va, val);
            } else {
                Load(pid, va);
            }
        }
        else if (choice == 3) {
            u64 pid;
            string hexVA;
            cout << "Enter PID and VA(Hex): ";
            cin >> pid >> hexVA;
            Visualize_Translation(pid, hex_to_int(hexVA));
            Print_TLB_State();
        }

    } while (choice != 0);

    return 0;
}