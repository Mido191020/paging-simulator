# System Architecture

## Core Components
The simulator mimics a Memory Management Unit (MMU) handling different paging strategies.

### Modules
1.  **Address Translation:** Logic to split Virtual Addresses into VPN and Offset.
2.  **Page Table:** Stores mappings from Virtual Page Numbers (VPN) to Physical Frame Numbers (PFN).
3.  **Replacement Policy:** Determines which page to evict when memory is full (demonstrated in later milestones).

## Data Flow
`Trace File (Input) -> CPU Wrapper -> MMU -> [Hit/Miss Check] -> Physical Memory Update`
