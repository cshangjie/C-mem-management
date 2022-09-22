#include "mem.h"
#include <stdio.h>

int main() {
    Initialize_Memory_Allocator(1600);
    Mem_Dump();
   
    char *p = Mem_Alloc(1591);
    if (p == NULL) {
        printf("Allocation failed\n");
        exit(0);
    }

    Mem_Dump();

    Mem_Free(p);
    Mem_Dump();
    
    Free_Memory_Allocator();
    return 0;
}
