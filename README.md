# Heap-management-in-c
Demo of Heap management with first and best-fit allocations methods

In this project, I allocated variables in the heap effectively in increasing order of address. If any free blocks are found consecutively, it combines all consecutive free blocks into one large free block.

BRIEF POINTS ABOUT HOW CODE RUNS

In This project, it contains 2 pointers (heap_lptr, free_lptr) which contains heap nodes and free nodes as double-linked list respectively.
It asks users to choose either for first-fit or best-fit allocation method.
for the first-fit method, we traverse the free list until we found a size greater than or equal to the size to be inserted. if we found then we insert it in the heap list, by taking space from free list and checking its corner cases (like if insertion takes place at the start or end or in the middle of the heap list)

similarly for best fit, same here we traverse the free list if we found the exact size then we insert or else if we found a size greater size than the size to be inserted we store a minimum of all sizes and at last, we insert in minimum size(because after insertion there should be no space left in the free block or space in the free block should be minimum)

hereafter every allocation, we sort the heap list because it gives us a display of allocation in increasing order, similarly for the free list after every deallocation because so the next incoming variables can be inserted in the least starting address.

it also contains many other functions like display_heap(display total free and heap blocks), display_all_free_blocks ......

at last, while exiting the program, it frees all heap blocks and free blocks and gives back memory to the system.
