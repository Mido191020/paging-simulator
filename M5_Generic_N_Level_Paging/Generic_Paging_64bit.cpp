#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <cstdint>

using namespace std;

// Type Alias for cleaner 64-bit code
typedef uint64_t u64;

/* ===================================================
   SECTION 1: System Configuration (64-Bit Arch)
   =================================================== */
const long long pageSize = 4096;
const long long MEM_SIZE = 131072;
const long long TOTAL_FRAMES = (MEM_SIZE / pageSize); // 32 Frames
const int RAM_SIZE = MEM_SIZE;

// 64-Bit Paging Geometry (4 Levels)
const int LEVELS = 4;
const int SHIFT_ARR[LEVELS] = {39, 30, 21, 12};
const u64 ENTRY_MASK = 0x1FF; // 9 bits (511)

// Error Codes
const int ERR_SEG_FAULT = -1;
const int ERR_PAGE_FAULT = -2;

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
   SECTION 3: N-Level Paging Structures
   =================================================== */

// Forward Declaration
struct PageTableV2;

// The Generic Entry (Union based)
struct PageTableEntryV2 {
    bool is_valid;
    union {
        struct PageTableV2* next_level_page_table; // Pointer for Branch nodes
        unsigned long long frame_number;             // Integer for Leaf nodes
    };
};

// The Generic Table
struct PageTableV2 {
    PageTableEntryV2 entries[512];
};

// Global Root Pointer (CR3 Register in x86)
PageTableV2* Root_Table = new PageTableV2;

/* ===================================================
   SECTION 4: Helpers (Bitwise Math)
   =================================================== */
u64 get_indexV2(u64 VA, int level) {
    return (VA >> SHIFT_ARR[level]) & ENTRY_MASK;
}

u64 get_offsetV2(u64 VA) {
    return VA & 0xFFF;
}

u64 Calculate_PA(u64 frame, u64 offset) {
    return (frame << 12) | offset;
}

/* ===================================================
   SECTION 5: The Engine (Iterative Logic)
   =================================================== */

// 1. The Builder (Iterative)
bool Handle_Page_FaultV2(u64 VA) {
    PageTableV2* current_table = Root_Table;

    for (int i = 0; i < LEVELS; ++i) {
        u64 idx = get_indexV2(VA, i);

        // If invalid, we must build/allocate
        if (current_table->entries[idx].is_valid == false) {

            // Case A: Leaf Node (Bottom Level)
            if (i == LEVELS - 1) {
                long long frame = allocate_frame(&physical_memory);
                if (frame < 0) return false; // Out of Memory

                current_table->entries[idx].frame_number = frame;
                current_table->entries[idx].is_valid = true;
                return true;
            }
                // Case B: Branch Node (Internal Level)
            else {
                PageTableV2* new_table = new PageTableV2();
                // CLEAN THE MEMORY (Crucial Step)
                for (int j = 0; j < 512; ++j) new_table->entries[j].is_valid = false;

                current_table->entries[idx].next_level_page_table = new_table;
                current_table->entries[idx].is_valid = true;
            }
        }

        // Navigation
        if (i < LEVELS - 1) {
            current_table = current_table->entries[idx].next_level_page_table;
        }
    }
    return true;
}

// 2. The Translator (Iterative)
long long TranslateV2(u64 VA) {
    PageTableV2* current_table = Root_Table;
    for (int i = 0; i < LEVELS; ++i) {
        u64 idx = get_indexV2(VA, i);

        if (current_table->entries[idx].is_valid == false) {
            return ERR_PAGE_FAULT;
        }

        if (i == LEVELS - 1) {
            u64 PFN = current_table->entries[idx].frame_number;
            return Calculate_PA(PFN, get_offsetV2(VA));
        } else {
            current_table = current_table->entries[idx].next_level_page_table;
        }
    }
    return ERR_PAGE_FAULT;
}

/* ===================================================
   SECTION 6: The "Plus" Feature (Recursive Logic) ➕
   =================================================== */

// Recursive Helper Function
long long Translate_Recursive_Helper(PageTableV2* table, u64 VA, int level) {
    // 1. Calculate Index
    u64 idx = get_indexV2(VA, level);

    // 2. Check Validity
    if (table->entries[idx].is_valid == false) {
        return ERR_PAGE_FAULT;
    }

    // 3. Base Case: Are we at the Leaf (Level 3)?
    if (level == LEVELS - 1) {
        u64 PFN = table->entries[idx].frame_number;
        return Calculate_PA(PFN, get_offsetV2(VA));
    }

    // 4. Recursive Step: Go Deeper
    return Translate_Recursive_Helper(table->entries[idx].next_level_page_table, VA, level + 1);
}

// Wrapper for the user
long long Translate_Recursive(u64 VA) {
    return Translate_Recursive_Helper(Root_Table, VA, 0);
}

/* ===================================================
   SECTION 7: Interface (Store/Load)
   =================================================== */

void Store(u64 VA, char data) {
    // Using Iterative Translator by default
    long long PA = TranslateV2(VA);

    if (PA == ERR_PAGE_FAULT) {
        // Handle Fault
        bool fixed = Handle_Page_FaultV2(VA);
        if (!fixed) {
            cout << "OOM Error!\n";
            return;
        }
        PA = TranslateV2(VA); // Retry
    }

    if (PA >= 0) {
        RAM[PA] = data;
        cout << "Stored '" << data << "' at PA 0x" << hex << PA << dec << "\n";
    }
}

char Load(u64 VA) {
    // Let's use the RECURSIVE translator here just to prove it works!
    long long PA = Translate_Recursive(VA);

    if (PA < 0) return '?';
    cout << "[DEBUG] Loading from RAM at " << PA << ": " << RAM[PA] << "\n";
    return RAM[PA];
}

u64 hex_to_int(string hex) {
    return stoull(hex, nullptr, 16);
}

/* ===================================================
   SECTION 8: Main
   =================================================== */

void Visualize_Translation_V2(u64 VA) {
    // Standard x86-64 Paging Names for display
    const string LevelNames[] = {
            "Level 4 (PML4)",
            "Level 3 (PDPT)",
            "Level 2 (PD)  ",
            "Level 1 (PT)  "
    };

    cout << "\n   [VISUALIZER 64-bit] Inspecting VA: 0x" << hex << VA << dec << "\n";
    cout << "   ==========================================================\n";

    PageTableV2* current_table = Root_Table;

    for (int i = 0; i < LEVELS; ++i) {
        // 1. Calculate Index
        u64 idx = get_indexV2(VA, i);

        cout << "   ├── " << LevelNames[i] << " | Index: " << idx << "\n";

        // 2. Check Validity
        if (current_table->entries[idx].is_valid == false) {
            cout << "   │   └── [X] Entry Invalid (Page Fault would occur here)\n";
            cout << "   ==========================================================\n";
            return;
        }

        // 3. Leaf or Branch?
        if (i == LEVELS - 1) {
            // Leaf Node (Level 1)
            u64 PFN = current_table->entries[idx].frame_number;
            u64 offset = get_offsetV2(VA);
            u64 PA = Calculate_PA(PFN, offset);

            cout << "   │   └── [OK] Leaf Found -> Frame Number: " << PFN << "\n";
            cout << "   │\n";
            cout << "   └── [RESULT] Physical Address: 0x" << hex << PA << dec << "\n";
        }
        else {
            // Branch Node (Levels 4, 3, 2)
            cout << "   │   └── [OK] Table Found -> Going deeper...\n";
            // Move down
            current_table = current_table->entries[idx].next_level_page_table;
        }
    }
    cout << "   ==========================================================\n";
}
void System_Boot() {
    init_free_list(&physical_memory);
    // Root Table is already allocated globally, but let's clean it
    for(int i=0; i<512; ++i) Root_Table->entries[i].is_valid = false;
    cout << "System Booted. Ready for 64-bit Paging.\n";
}



void run_batch_test() {
    cout << "\n=== RUNNING 64-BIT BATCH TEST ===\n";
    ofstream out("input.txt");
    // Write test cases including Huge Addresses
    out << "W 0x1000 A\n"           // Standard small address
           "R 0x1000\n"
           "W 0x1A00200300 B\n"     // Huge 64-bit address (Level 4 usage)
           "R 0x1A00200300\n"
           "R 0x9999999999\n";      // Fault test
    out.close();

    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) return;

    string virtual_address_HEX;
    char operation, data;

    while (inputFile >> operation >> virtual_address_HEX) {
        u64 VA = hex_to_int(virtual_address_HEX);
        cout << "\nCOMMAND: " << operation << " " << virtual_address_HEX << "\n";

        if (operation == 'W') {
            inputFile >> data;
            Store(VA, data);
        } else {
            Load(VA);
        }
    }
    inputFile.close();
    cout << "=== BATCH TEST COMPLETE ===\n\n";
}

int main() {
    System_Boot();

    int choice;
    do {
        cout << "\n========================================\n";
        cout << "   M3.5 N-Level Paging Explorer (64-bit) \n";
        cout << "========================================\n";
        cout << "1. Run Batch Test (input.txt)\n";
        cout << "2. Interactive Mode (Store/Load)\n";
        cout << "3. Visualize Translation (Tree Walk)\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            run_batch_test();
        }
        else if (choice == 2) {
            string hexAddr;
            char op, val;
            cout << "Enter Operation (W/R): ";
            cin >> op;
            cout << "Enter Address (Hex): ";
            cin >> hexAddr;

            u64 VA = hex_to_int(hexAddr);

            if (op == 'W' || op == 'w') {
                cout << "Enter Value (Char): ";
                cin >> val;
                Store(VA, val);
            } else {
                Load(VA);
            }
        }
        else if (choice == 3) {
            string hexAddr;
            cout << "Enter Address to Inspect (Hex): ";
            cin >> hexAddr;

            u64 VA = hex_to_int(hexAddr);
            Visualize_Translation_V2(VA);
        }

    } while (choice != 0);

    return 0;
}