////////////////////////////////
// Seth Risner and Connor Smith
////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MEMORY_SIZE 256
#define LARGE_MEMORY_SIZE (1 * 1024 * 1024) // 1 MB
#define BAD_BLOCK 'X'

// Simulate the memory space for our allocator
static char memory[MEMORY_SIZE];

// Simulate a large memory space for testing
char *large_memory = NULL;

// Structure to manage memory blocks
typedef struct Block {
    size_t size;
    bool free;
    struct Block *next;
} Block;

Block *freeList = (Block*)memory;

// Initialize the memory manager
void initializeMemory() {
    freeList->size = MEMORY_SIZE - sizeof(Block);   // sizeof(Block) = 24 // 256 - 24 = 232
    freeList->free = true;
    freeList->next = NULL;
}

// Randomly mark blocks as "bad" in the large memory
void markBadBlocks(char *memory, size_t size, size_t badBlockCount) {
    srand(time(NULL));
    for (size_t i = 0; i < badBlockCount; i++) {
        size_t randomIndex = rand() % size;
        memory[randomIndex] = BAD_BLOCK; // Mark as bad block
    }
}

// Skeleton function: Allocate memory dynamically, skipping bad blocks
void* myMalloc(size_t size) {
    // STUDENTS: Implement logic to allocate memory dynamically, ensuring that you skip over bad blocks
    
/*
    myMalloc:
    1. create a character pointer array to copy good data from large memory
    2. create a copy of freeList to iterate through freeList in parallel to large memory
    3. iterate through large memory
        - check for if the block is free
        - check for if the block is a bad block AKA if large_memory[i] != 'X'
        - if all are true, copy &large_memory[i] into allocated_memory[i]
    4. Edge cases:
        do you have enough usable memory?

    myFree():
        - if the void* is NULL, nothing needs to be freed so just return
        - check if the current block on freeList is free. mark freeList->free = true
        - make sure current block and next block is free, if both are free, then set the size of current block to sizeof(currentblock) += current->next->size
        - myFree is combining blocks into one
        - while (current != NULL && current->next != null)
*/

    // 1 & 2
    char* allocated_memory = NULL;
    Block* head = freeList;
    int block_count {0}; // tracker for how many good blocks have been allocated, if block_count exceeds the size the user input, return allocated_memory

    // 3
    while (head->next != NULL) {
        for (size_t i {0}; i < LARGE_MEMORY_SIZE; i++) {
            if (head->free == true && large_memory[i] != BAD_BLOCK) {
                allocated_memory = &large_memory[i];
            }
            if (block_count >= size) {
                return allocated_memory;
            }
        
            head = head->next;
        }
    }
    
    
    

    
    return NULL; // Placeholder return value
}

// Skeleton function: Free the allocated memory
void myFree(void *ptr) {
    // STUDENTS: Implement logic to free the allocated memory
}

int main() {

    size_t allocationSize = 10;

    // Initialize memory management
    initializeMemory();

    // Allocate a large memory block (1 MB)
    large_memory = (char*)malloc(LARGE_MEMORY_SIZE);
    if (large_memory == NULL) {
        printf("Failed to allocate large memory.\n");
        return 1;
    }

    for (size_t i {0}; i < LARGE_MEMORY_SIZE; i++) {
        large_memory[i] = 'W';
    }

    // Mark some blocks as "bad"
    markBadBlocks(large_memory, LARGE_MEMORY_SIZE, 1000); // Mark 1000 bad blocks

    // Simulate memory allocation
    int *array = (int*)myMalloc(allocationSize * sizeof(int));  // Allocate memory for an array of integers
    if (array == NULL) {
        printf("Memory allocation failed.\n");
    } else {
        // Assign values to the array and print them
        for (size_t i = 0; i < allocationSize; i++) {
            array[i] = i * i;  // Assign square of index
            printf("Array[%llu] = %d\n", i, array[i]);
        }

        // Free the allocated memory
        myFree(array);
        printf("Memory successfully freed.\n");
    }

    // Clean up large memory block using system's free function
    myFree(large_memory);

    // REMEBER TO REMOVE THIS AFTER myFree() HAS BEEN COMPLETED
    delete[] array;
    free(large_memory);

    return 0;
}