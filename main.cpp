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
   SECTION 3: TLB Simulator
   =================================================== */









/* ===================================================
   SECTION 7: Interface & Main
   =================================================== */

void Store(u64 PID, u64 VA, char data) {
   //TODO:
}

char Load(u64 PID, u64 VA) {

    //TODO:
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
            //TODO:
        }
    }
    inputFile.close();
}

void System_Boot() {
    init_free_list(&physical_memory);
    //TODO:
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
            //TODO:
        }

    } while (choice != 0);

    return 0;
}