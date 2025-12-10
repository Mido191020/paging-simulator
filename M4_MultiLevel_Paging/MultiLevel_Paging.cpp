#include <iostream>
#include <string>
#include <cassert>
#include <fstream>

using namespace std;

/* ===================================================
   SECTION 1: System Constants & Global Config
   =================================================== */
const long long pageSize = 4096;
const long long MEM_SIZE = 131072;
const long long TOTAL_FRAMES = (MEM_SIZE / pageSize); // 32 Frames
const int RAM_SIZE = MEM_SIZE;

// M3 Constants (10-10-12 Paging)
const int page_table_array_size = 1024;

// Error Codes
const int ERR_SEG_FAULT = -1;
const int ERR_PAGE_FAULT = -2;
string ERR_SEG_FAULT_Message = "Error: Segmentation Fault (Address too high)\n";
string ERR_PAGE_FAULT_Message = "Error: Page Fault (Page not in memory)\n";

/* ===================================================
   SECTION 2: Physical Memory (Hardware Layer)
   =================================================== */
// The actual RAM storage
unsigned char RAM[RAM_SIZE];

/* ===================================================
   SECTION 3: Frame Allocator (Free List)
   =================================================== */
typedef struct {
    int fram[TOTAL_FRAMES];
    int top;
} freeList;

// Global Physical Memory Manager
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
   SECTION 4: Multi-Level Paging Structures (M3 Design)
   =================================================== */
struct PageTableEntry {
    long long frame_number;
    bool is_valid;
};

struct PageTable {
    PageTableEntry entries[page_table_array_size];
};

struct PageDirectory {
    PageTable* tables[page_table_array_size];
};

// The Root of the Tree
PageDirectory* Directory_table = new PageDirectory;

void init_Directory_table() {
    for (int i = 0; i < page_table_array_size; ++i) {
        Directory_table->tables[i] = NULL;
    }
}

/* ===================================================
   SECTION 5: Bitwise Helpers
   =================================================== */
unsigned int get_directory_index(unsigned int virtual_address) {
    return virtual_address >> 22;
}

unsigned int get_tabel_index(unsigned int virtual_address) {
    return (virtual_address >> 12) & 0x3FF;
}

unsigned int get_offset_index(unsigned int virtual_address) {
    return virtual_address & 0xFFF;
}

long long Compute_PA(int frame_number, long long offset) {
    return (frame_number << 12) | offset;
}

/* ===================================================
   SECTION 6: M3 Core Logic (Translation & Handling)
   =================================================== */

bool Check_Directory(unsigned int Directory_Index) {
    if (Directory_table->tables[Directory_Index] == NULL) {
        return false;
    }
    return true;
}

bool Check_Page_Table_Entry(unsigned int Directory_Index, unsigned int Table_Index) {
    if(Directory_table->tables[Directory_Index]->entries[Table_Index].is_valid){
        return true;
    }
    return false;
}

bool Handle_Page_FaultM3(long long VA){
    unsigned int Directory_Index = get_directory_index(VA);
    unsigned int Table_Index = get_tabel_index(VA);

    // 1. Directory Level Allocation (Build the Floor)
    if (!Check_Directory(Directory_Index)){
        PageTable* new_table = new PageTable();
        Directory_table->tables[Directory_Index] = new_table;

        for (int i = 0; i < page_table_array_size; ++i) {
            new_table->entries[i].is_valid= false;
        }
    }

    // 2. Table Level Allocation (Furnish the Office)
    PageTable* current_table = Directory_table->tables[Directory_Index];
    if (!Check_Page_Table_Entry(Directory_Index, Table_Index)){
        int frame = allocate_frame(&physical_memory);
        if (frame == -1){
            return false; // Out of Memory
        }
        current_table->entries[Table_Index].frame_number = frame;
        current_table->entries[Table_Index].is_valid = true;
    }
    return true;
}

long long Translate_Address_M3(long long VA) {
    unsigned int Directory_Index = get_directory_index(VA);
    unsigned int Table_Index = get_tabel_index(VA);
    unsigned int Offset = get_offset_index(VA);

    // Tree Walk Logic
    if (!Check_Directory(Directory_Index) || !Check_Page_Table_Entry(Directory_Index, Table_Index)) {
        bool fixed = Handle_Page_FaultM3(VA);
        if (!fixed) {
            return ERR_PAGE_FAULT;
        }
    }

    int frameNumber = Directory_table->tables[Directory_Index]->entries[Table_Index].frame_number;
    return Compute_PA(frameNumber, Offset);
}

/* ===================================================
   SECTION 7: Helpers & I/O
   =================================================== */
long long translation(string VirtualAddressHex) {
    return stoll(VirtualAddressHex, nullptr, 16);
}

long long Get_Physical_Address(long long VA) {
    return Translate_Address_M3(VA);
}

void Store(long long VA, char data) {
    long long PA = Get_Physical_Address(VA);
    if (PA >= 0) {
        RAM[PA] = data;
        cout << "Stored '" << data << "' at Physical Address 0x" << hex << PA << dec << "\n";
    }
}

char load(long long VA) {
    long long PA = Get_Physical_Address(VA);
    if (PA < 0) return '?';
    cout << "[DEBUG] Loading from RAM at " << PA << ": " << RAM[PA] << "\n";
    return RAM[PA];
}

/* ===================================================
   SECTION 8: Modes & UI (Visualizer, Batch, Interactive)
   =================================================== */

void Visualize_Translation(long long VA) {
    unsigned int Dir_Idx = get_directory_index(VA);
    unsigned int Tab_Idx = get_tabel_index(VA);
    unsigned int Offset  = get_offset_index(VA);

    cout << "\n   [VISUALIZER] Inspecting VA: 0x" << hex << VA << dec << "\n";
    cout << "   ├── 1. Directory Index: " << Dir_Idx << "\n";

    if (!Check_Directory(Dir_Idx)) {
        cout << "   ├── [X] Directory Entry is NULL (Unmapped Region)\n";
        cout << "   └── [Result] Page Fault would occur here.\n";
        return;
    }
    cout << "   ├── [OK] Page Table Found.\n";

    cout << "   ├── 2. Table Index: " << Tab_Idx << "\n";
    if (!Check_Page_Table_Entry(Dir_Idx, Tab_Idx)) {
        cout << "   ├── [X] Page Table Entry is Invalid (Page not loaded)\n";
        cout << "   └── [Result] Page Fault would occur here.\n";
        return;
    }
    cout << "   ├── [OK] Frame Found (Valid Bit = 1).\n";

    int frame = Directory_table->tables[Dir_Idx]->entries[Tab_Idx].frame_number;
    cout << "   └── 3. Physical Frame: " << frame << " | Offset: " << Offset << "\n";
    cout << "       -> Physical Address: 0x" << hex << Compute_PA(frame, Offset) << dec << "\n";
}

void run_batch_test() {
    cout << "\n=== RUNNING BATCH TEST FROM input.txt ===\n";
    ofstream out("input.txt");
    out << "W 0x1000 A\n"
           "R 0x1000\n"
           "W 0x400000 B\n" // Far jump to test Directory Index change
           "R 0x999999\n";
    out.close();

    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) return;

    string virtual_address_HEX;
    char operation, data;

    while (inputFile >> operation >> virtual_address_HEX) {
        long long VA = translation(virtual_address_HEX);
        cout << "\nCOMMAND: " << operation << " " << virtual_address_HEX << "\n";

        if (operation == 'W') {
            inputFile >> data;
            Store(VA, data);
        } else {
            load(VA);
        }
    }
    inputFile.close();
    cout << "=== BATCH TEST COMPLETE ===\n\n";
}

void System_Boot() {
    init_free_list(&physical_memory);
    init_Directory_table();
    cout << "System Booted. Memory Empty. Ready for Multi-Level Paging.\n";
}

int main() {
    System_Boot();

    int choice;
    do {
        cout << "\n====================================\n";
        cout << "   M3 Multi-Level Paging Explorer   \n";
        cout << "====================================\n";
        cout << "1. Run Batch Test (input.txt)\n";
        cout << "2. Interactive Mode (Store/Load)\n";
        cout << "3. Visualize Translation (Debug Tree Walk)\n";
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

            long long VA = translation(hexAddr);

            if (op == 'W' || op == 'w') {
                cout << "Enter Value (Char): ";
                cin >> val;
                Store(VA, val);
            } else {
                load(VA);
            }
        }
        else if (choice == 3) {
            string hexAddr;
            cout << "Enter Address to Inspect (Hex): ";
            cin >> hexAddr;

            long long VA = translation(hexAddr);
            Visualize_Translation(VA);
        }

    } while (choice != 0);

    return 0;
}
