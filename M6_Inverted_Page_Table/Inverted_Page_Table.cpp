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
// We keep it small (10) to FORCE collisions and see the Linked List work.
const int TABLE_SIZE = 10;

// Error Codes
const u64 ERR_PAGE_FAULT = -1;

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
   SECTION 3: Inverted Page Table Structures
   =================================================== */

struct Node {
    u64 PID;   // Owner Process
    u64 VPN;   // Virtual Page Number
    u64 PFN;   // Physical Frame Number
    Node* next; // Chaining for collisions
};

struct HashTable {
    Node* buckets[TABLE_SIZE];
};

// Global Inverted Page Table
HashTable* System_IPT = new HashTable;

void Init_IPT() {
    for(int i=0; i<TABLE_SIZE; i++) {
        System_IPT->buckets[i] = NULL;
    }
}

/* ===================================================
   SECTION 4: Helpers (Hash & Math)
   =================================================== */

u64 get_VPN(u64 VA) {
    return VA >> 12; // Remove Offset (Bottom 12 bits)
}

u64 get_offset(u64 VA) {
    return VA & 0xFFF; // Keep only Offset
}

u64 Hash_Function(u64 PID, u64 VPN) {
    // Simple XOR Hash
    u64 combined = PID ^ VPN;
    return combined % TABLE_SIZE; // Force into [0..9]
}

u64 construct_PA(u64 frame, u64 offset) {
    return (frame << 12) | offset;
}

/* ===================================================
   SECTION 5: Core Logic (Lookup & Insert)
   =================================================== */

Node* find_node(Node* head, u64 PID, u64 VPN) {
    Node* current = head;
    while (current != NULL) {
        if (current->PID == PID && current->VPN == VPN) {
            return current; // Found match
        }
        current = current->next;
    }
    return NULL; // Not found
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

    // Link it
    new_node->next = System_IPT->buckets[idx];
    System_IPT->buckets[idx] = new_node;
}

u64 Translate_Inverted(u64 PID, u64 VA) {
    u64 vpn = get_VPN(VA);
    u64 offset = get_offset(VA);
    u64 idx = Hash_Function(PID, vpn);

    // 1. Lookup
    Node* target = find_node(System_IPT->buckets[idx], PID, vpn);

    // 2. Handle Page Fault
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
        // Silent allocation (use Visualizer to see details)
    }

    return construct_PA(pfn, offset);
}

/* ===================================================
   SECTION 6: The Journey Visualizer 🕵️‍♂️
   =================================================== */
void Visualize_Inverted(u64 PID, u64 VA) {
    u64 vpn = get_VPN(VA);
    u64 offset = get_offset(VA);
    u64 idx = Hash_Function(PID, vpn);

    cout << "\n   [VISUALIZER] Journey of (PID: " << PID << ", VA: 0x" << hex << VA << dec << ")\n";
    cout << "   =========================================================\n";

    // Step 1: Logic
    cout << "   1. Extraction: VPN = " << vpn << " | Offset = 0x" << hex << offset << dec << "\n";
    cout << "   2. Hashing   : (" << PID << " ^ " << vpn << ") % 10 = Index [" << idx << "]\n";

    // Step 2: The Bucket
    cout << "   3. Bucket [" << idx << "] Access:\n";
    Node* current = System_IPT->buckets[idx];

    if (current == NULL) {
        cout << "      └── [EMPTY] Bucket is empty.\n";
        cout << "      └── [RESULT] PAGE FAULT (New entry will be created here).\n";
        cout << "   =========================================================\n";
        return;
    }

    // Step 3: Walking the Chain
    int step = 1;
    bool found = false;
    while (current != NULL) {
        cout << "      └── Node " << step++ << ": [PID:" << current->PID << ", VPN:" << current->VPN << "] ";

        if (current->PID == PID && current->VPN == vpn) {
            cout << "--> MATCH! ✅ (PFN: " << current->PFN << ")\n";
            found = true;
            break;
        } else {
            cout << "--> No Match ❌ (Next...)\n";
        }
        current = current->next;
    }

    // Step 4: Conclusion
    if (found) {
        u64 pa = construct_PA(current->PFN, offset);
        cout << "   4. Calculation: (Frame " << current->PFN << " << 12) | " << offset << "\n";
        cout << "   └── [RESULT] Physical Address: 0x" << hex << pa << dec << "\n";
    } else {
        cout << "      └── [END OF LIST] No match found.\n";
        cout << "      └── [RESULT] PAGE FAULT.\n";
    }
    cout << "   =========================================================\n";
}

/* ===================================================
   SECTION 7: Interface & Main
   =================================================== */

void Store(u64 PID, u64 VA, char data) {
    u64 PA = Translate_Inverted(PID, VA);
    if (PA != ERR_PAGE_FAULT) {
        RAM[PA] = data;
        cout << "   [RAM] PID " << PID << " stored '" << data << "' at PA 0x" << hex << PA << dec << "\n";
    }
}

char Load(u64 PID, u64 VA) {
    u64 PA = Translate_Inverted(PID, VA);
    if (PA != ERR_PAGE_FAULT) {
        cout << "   [RAM] PID " << PID << " loaded '" << RAM[PA] << "' from PA 0x" << hex << PA << dec << "\n";
        return RAM[PA];
    }
    return '?';
}

u64 hex_to_int(string hex) {
    return stoull(hex, nullptr, 16);
}

void run_batch_test() {
    cout << "\n=== RUNNING INVERTED BATCH TEST ===\n";
    cout << "Format: Op PID VA [Value]\n";

    ofstream out("input_ipt.txt");
    // Collision Test Case:
    // PID 1, VPN 10 (0xA000) -> Hash (1^10)%10 = 11%10 = 1
    // PID 1, VPN 0  (0x0000) -> Hash (1^0)%10  = 1%10  = 1 (COLLISION!)
    out << "W 1 0xA000 X\n"
           "W 1 0x0000 Y\n"
           "R 1 0xA000\n"
           "R 1 0x0000\n"
           "V 1 0xA000\n"; // V for Visualize
    out.close();

    ifstream inputFile("input_ipt.txt");
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
            Visualize_Inverted(pid, VA);
        }
    }
    inputFile.close();
}

void System_Boot() {
    init_free_list(&physical_memory);
    Init_IPT();
    cout << "System Booted. Inverted Page Table (Size=" << TABLE_SIZE << ") Ready.\n";
}

int main() {
    System_Boot();

    int choice;
    do {
        cout << "\n========================================\n";
        cout << "   M4 Inverted Page Table Explorer      \n";
        cout << "========================================\n";
        cout << "1. Run Batch Test (Collisions)\n";
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
            Visualize_Inverted(pid, hex_to_int(hexVA));
        }

    } while (choice != 0);

    return 0;
}