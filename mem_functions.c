#include "mem.h"
extern BLOCK_HEADER* first_header;
int Check_Alloc(int size_alloc);
BLOCK_HEADER* Next_Block(BLOCK_HEADER* block);
int blockHeaderSize = 8;


// return a pointer to the payload
// if a large enough free block isn't available, return NULL
void* Mem_Alloc(int size){
    BLOCK_HEADER* current = first_header;
    int userPayload = size;

    // find a free block that's big enough
    while(1){
        // return NULL if we didn't find a block
        if(current->size_alloc == 1){
            return NULL;
        }

        // check if free
        if(Check_Alloc(current->size_alloc) == 0){
            // check if the free block will fit the request
            if(current->payload >= userPayload){
                // break if block found
                break;
            }
        }
        // move to next block
        current = Next_Block(current);
    }

    // calculate padding needed
    int padding = current->size_alloc - userPayload - blockHeaderSize;
     printf("%s%d\n", "Padding: ",padding);
    // store the current size alloc
    int current_alloc = current->size_alloc;

    // split if necessary (if padding size is greater than or equal to 16 split the block)
    if(padding >= 16){
        // Edit old block
        int currSizeAlloc = blockHeaderSize + userPayload;
        // round up to 16
        while (currSizeAlloc%16) currSizeAlloc++;
        // update the current block header and mark as allocated
        current->size_alloc = currSizeAlloc + 1;
        current->payload = userPayload;
        // create new block header with correct values
        BLOCK_HEADER* newBlock = Next_Block(current);
        newBlock->size_alloc = current_alloc - currSizeAlloc;
        newBlock->payload = current_alloc - currSizeAlloc - blockHeaderSize;
    }
    else{
        // Mark block as allocated and update payload
        current->size_alloc += 1;
        current->payload = userPayload; // I ADDED THIS
    }
    // return the pointer
    return (void *)((unsigned long)current+8);;
}

/**
 * Check if size_alloc odd or even, if odd then alloc bit = 1 and is allocated
 * Otherwise allocbit will = 0 indicating is free
 * 
 * @param size_alloc 
 * @return int 
 */
int Check_Alloc(int size_alloc){
    int alloc_bit = size_alloc % 2;
    return alloc_bit;
}
/**
 * Find next block given current block
 * 
 * @param block 
 * @return BLOCK_HEADER* 
 */
BLOCK_HEADER* Next_Block(BLOCK_HEADER* block){
    int block_size = block->size_alloc & 0xFFFFFFFE;
    block = (BLOCK_HEADER *)((unsigned long)block + block_size);
    return block;
}
void Coalesce_Current(BLOCK_HEADER* prevBlock,BLOCK_HEADER* current, BLOCK_HEADER* postBlock){
    // Check that postBlock is not the last block and that it is indeed free
    int postFlag = 0;
    int preFlag = 0;
    if((postBlock->size_alloc != 1) && Check_Alloc(postBlock->size_alloc) == 0){
        //printf("%s\n","coalescing with post block");
        // update the current block
        int currentPadding = (current->size_alloc) - blockHeaderSize - current->payload;
        current->size_alloc = current->size_alloc + postBlock->size_alloc;
        current->payload = current->payload + postBlock->size_alloc + currentPadding;
        postFlag = 1;
    }
    // Check that prevBlock isn't null and that it is free
    if(prevBlock!=NULL && Check_Alloc(prevBlock->size_alloc) == 0){
        //printf("%s\n","coalescing with prev block");
        prevBlock->size_alloc = prevBlock->size_alloc + current->size_alloc;
        prevBlock->payload = prevBlock->payload + current->size_alloc;
        preFlag = 1;
    }
    if(preFlag == 0 && postFlag == 0) {
        current->payload = current->size_alloc - blockHeaderSize;
    }
}


// return 0 on success
// return -1 if the input ptr was invalid
int Mem_Free(void *ptr){
    // check if the passed ptr is null or not divisible by 16
    // if(ptr == NULL || (ptr % 16 != 0)){
    //     return -1;
    // }
    // traverse the list and check all pointers to find the correct block 
    // if you reach the end of the list without finding it return -1
    BLOCK_HEADER* current = first_header;
    BLOCK_HEADER* prevBlock = NULL;
    BLOCK_HEADER* postBlock = NULL;
    while(1){
        // return -1 on last block since not found 
        if(current->size_alloc == 1){
            //printf("%s\n", "quitting while");
            return -1;
        }
        // find correct block
        //printf("%p\n",ptr);
        //printf("%p\n",current);
        if(ptr == (void*)((unsigned long)current + 8)){
            break;
        }
        else{
            // update prevBlock
            prevBlock = current;
            current = Next_Block(current);
        }
    }
    //printf("%s\n","block found");
    // fetch the post block after current correct block is determined
    postBlock = Next_Block(current);

    // if block is already freed check for coalesce
    if(Check_Alloc(current->size_alloc) == 0){
        Coalesce_Current(prevBlock, current, postBlock);
        return 0;
    }

    // free the block by setting alloc bit to 0
    current->size_alloc = current->size_alloc - 1;

    // coalesce adjacent free blocks
    Coalesce_Current(prevBlock, current, postBlock);

    return 0;
}

