#include <iostream>
#include <string>
#include <cassert>

using namespace std;
const long long pageSize = 4096;
const long long MEM_SIZE =131072;
const long long TOTAL_FRAMES= (MEM_SIZE / pageSize);//32 Frames
const int MAX_VIRTUAL_PAGES = 1000;



typedef struct{
    int fram[TOTAL_FRAMES];
   int top;
} freeList  ;
void init_free_list(freeList*list){
    list->top=-1;
    for (int i = 0; i < TOTAL_FRAMES; ++i) {
        list->top++;
        list->fram[list->top]=i;
    }
}
int allocate_frame(freeList*list){
    if (list->top==-1)return -1;
    int allocated_frame = list->fram[list->top];
    list->top--;
    return allocated_frame;
}
void free_frame(freeList*list,int frameNumber){
    if (list->top==TOTAL_FRAMES-1)return;
    list->top++;
    list->fram[list->top]=frameNumber;
}

struct PageTableEntry {
    int frame_number;  // The output value (e.g., 12)
    bool is_valid;     // Is this mapping active? (true/false)
};
struct PageTableEntry my_page_table[MAX_VIRTUAL_PAGES];
void init_page_table(){
    for (int i = 0; i < MAX_VIRTUAL_PAGES; ++i) {
        my_page_table[i].is_valid= false;
    }
}


long long translation(string VirtualAddressHex){
    return stoll(VirtualAddressHex, nullptr, 16);;
}
long long Compute_page_number(long long VA){
    return VA / pageSize;
}
long long Compute_offset(long long VA){
   return VA % pageSize;
}
int Mping_Frame(){

}

long long Compute_PA(long long frame_number,long long offset){
    return (frame_number * pageSize) + offset;
}
int main() {



    cout << "enter your virtual address (in hex): ";
    string virtual_address_HEX;
    cin >> virtual_address_HEX;

    // convert -> int
    long long VA = translation(virtual_address_HEX);

    cout << "your Virtual_address is: " << VA << "\n";

    long long page_number = Compute_page_number(VA);
    assert(page_number <MAX_VIRTUAL_PAGES && "Segmentation Fault: Address out of bounds");
    long long offset = Compute_offset(VA) ;

    int frame_number = page_number;           // identity mapping
    long long PA = Compute_PA(frame_number,offset);

    return 0;
}


/*do {
// MENU
cout << "\n";
cout << "==================================\n";
cout << "   Virtual Address Translator\n";
cout << "   (Identity Mapping - 4KB pages)\n";
cout << "==================================\n";
cout << "1 - Enter a new virtual address\n";
cout << "0 - Exit\n";
cout << "Choice: ";
cin >> choice;

if (choice == 0) {
cout << "Goodbye!\n";
break;
}

if (choice != 1) {
cout << "Invalid option! Please enter 1 or 0.\n";
continue;
}

// Your original code starts here

// Your original output + a bit nicer
cout << "Page number   : " << page_number << "\n";
cout << "Offset        : " << offset << "\n";
cout << "Physical Address : 0x" << hex << PA << dec << "  (=" << PA << ")\n\n";

} while (true);*/