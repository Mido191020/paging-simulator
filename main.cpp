#include <iostream>
#include <string>
#include <cassert>

using namespace std;

/* =========================
   Global Constants   ========================= */
const long long pageSize = 4096;
const long long MEM_SIZE = 131072;
const long long TOTAL_FRAMES = (MEM_SIZE / pageSize); // 32 Frames
const int MAX_VIRTUAL_PAGES = 1000;
// Error Codes
//const int SUCCESS = 1;          // Or we just return the frame number if >= 0
const int ERR_SEG_FAULT = -1;   // Address out of bounds
const int ERR_PAGE_FAULT = -2;  // Page not present
string ERR_SEG_FAULT_Message="Error: Segmentation Fault (Address too high)\n";
string ERR_PAGE_FAULT_Message="Error: Page Fault (Page not in memory)\n";

/* =========================
   Free Frame List

========================= */


typedef struct {
    int fram[TOTAL_FRAMES];
    int top;
} freeList;

void init_free_list(freeList* list) {
    list->top = -1;
    for (int i = 0; i < TOTAL_FRAMES; ++i) {
        list->top++;
        list->fram[list->top] = i;
    }
}

// Remove from "Free" list (Pop)
long long allocate_frame(freeList* list) {
    if (list->top == -1) return -1;

    int allocated_frame = list->fram[list->top];
    list->top--;
    return allocated_frame;
}

// Free = Add to "Free" list (Push)
void free_frame(freeList* list, int frameNumber) {
    if (list->top == TOTAL_FRAMES - 1) return;

    list->top++;
    list->fram[list->top] = frameNumber;
}

/* =========================
   Page Table
========================= */
struct PageTableEntry {
    long long frame_number; // physical frame number
    bool is_valid;          // valid / invalid
};

PageTableEntry my_page_table[MAX_VIRTUAL_PAGES];
freeList physical_memory;

void init_page_table() {
    for (int i = 0; i < MAX_VIRTUAL_PAGES; ++i) {
        my_page_table[i].is_valid = false;
    }
}

long long Mapping_FrameWithPageTable(long long page_number) {
    if (page_number > MAX_VIRTUAL_PAGES ){
        cout<<"Segmentation Fault: Address out of bounds";
        return ERR_SEG_FAULT;
    }
    if (my_page_table[page_number].is_valid == false){
        cout<< "Error: Page Fault (Page not mapped/present)";
        return ERR_PAGE_FAULT;
    }

    return my_page_table[page_number].frame_number;
}
//Determine Cause of Fault
//handel Address out of bounds
//handel Page Fault (Page not mapped/present)
bool Handle_Page_Fault(int page_number){

}




/* =========================
   Address Translation Helpers
========================= */
long long translation(string VirtualAddressHex) {
    return stoll(VirtualAddressHex, nullptr, 16);
}

long long Compute_page_number(long long VA) {
    return VA / pageSize;
}

long long Compute_offset(long long VA) {
    return VA % pageSize;
}

long long Compute_PA(int frame_number, long long offset) {
    return (frame_number * pageSize) + offset;
}

/* =========================
   System Boot
========================= */
void System_Boot() {
    init_free_list(&physical_memory);
    init_page_table();

    // Map first 5 virtual pages
    for (int i = 0; i < 5; ++i) {
        int pfn = allocate_frame(&physical_memory);

        if (pfn != -1) {
            my_page_table[i].frame_number = pfn;
            my_page_table[i].is_valid = true;

            cout << "  -> Mapped Virtual Page "
                 << i
                 << " to Physical Frame "
                 << pfn << "\n";
        } else {
            cout << "  -> Failed to map Page "
                 << i
                 << " (Out of Memory)\n";
        }
    }
}

void menu() {
}

/* =========================
   Main
========================= */
int main() {

    init_page_table();
    System_Boot();

    do {
        cout << "\n";
        cout << "==================================\n";
        cout << "   Virtual Address Translator\n";
        cout << "   (Identity Mapping - 4KB pages)\n";
        cout << "==================================\n";
        cout << "1 - Enter a new virtual address\n";
        cout << "0 - Exit\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        if (choice == 0) {
            cout << "Goodbye!\n";
            break;
        }

        if (choice != 1) {
            cout << "Invalid option! Please enter 1 or 0.\n";
            continue;
        }

        cout << "enter your virtual address (in hex): ";
        string virtual_address_HEX;
        cin >> virtual_address_HEX;

        // Translate hex → integer
        long long VA = translation(virtual_address_HEX);

        cout << "your Virtual_address is: " << VA << "\n";

        long long page_number = Compute_page_number(VA);
        long long offset      = Compute_offset(VA);

        int frame_number      = Mapping_FrameWithPageTable(page_number);
        if (frame_number<0){
            if (frame_number==ERR_PAGE_FAULT){
                cout<<ERR_PAGE_FAULT_Message;
            }
            if (frame_number==ERR_SEG_FAULT){
                cout<<ERR_SEG_FAULT_Message;
            }
            continue;
        }
        // Identity mapping
        long long PA = Compute_PA(frame_number, offset);

        cout << "Page number       : " << page_number << "\n";
        cout << "Offset            : " << offset << "\n";
        cout << "Physical Address  : 0x"
             << hex << PA << dec
             << "  (=" << PA << ")\n\n";

    } while (true);

    return 0;
}