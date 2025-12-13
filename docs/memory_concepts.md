# Memory Concepts

## Supported Paging Modes
This project explores the evolution of paging techniques:

1.  **Linear Paging:** A single array mapping VPN to PFN. Simple but memory-heavy.
2.  **Multi-Level Paging:** Breaks the page table into a tree structure (Page Directory -> Page Table) to save space.
3.  **Inverted Page Table:** Maps Physical Frames to Virtual Pages, scaling better for large address spaces (64-bit).
