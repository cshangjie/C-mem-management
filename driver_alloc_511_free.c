#include "mem.h"
#include <stdio.h>

int main() {
    Initialize_Memory_Allocator(1600);
    Mem_Dump();
   
    char *p = Mem_Alloc(511);
    if (p == NULL) {
        printf("Allocation failed\n");
        exit(0);
    }
    for (int i = 0; i < 512; i++){
        p[i] = 'c';
    }

    Mem_Dump();

    Mem_Free(p);
    Mem_Dump();
    
    Free_Memory_Allocator();
    return 0;
}
