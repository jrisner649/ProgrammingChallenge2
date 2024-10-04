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

    // We must return null if we don't have enough memory to allocate
    if (size > LARGE_MEMORY_SIZE) {
        return NULL;
    }
    
    Block* head = freeList;
    Block* previous;
    size_t address_count {0};    // Keep track of how many good contigous memory addresses we have found
    bool is_bad {false};         // flag to indicate if the data is bad

    // Divide the memory into blocks of size 256
    for (size_t i {0}; i < LARGE_MEMORY_SIZE; i++, address_count++) {
        // If we find bad data, then we will set the block's free member to false later
        if (large_memory[i] == BAD_BLOCK) {
            is_bad = true;
        }
        if (address_count == MEMORY_SIZE) {
            Block memory_block {MEMORY_SIZE, is_bad, NULL}; // Create a new block of size MEMORY_SIZE, mark if it is free or not depending on if we found a BAD_BLOCK, and initialize its next pointer to NULL
            previous->next = &memory_block;
            previous = &memory_block;
        }
    }

   

    char* start = &large_memory[0]; // Store the address of the first element of large memory
                                    // We will use this variable to store the starting address of a contigouous block of memory
    

    // Iterate through the large memory
    for (size_t i {0}; i < LARGE_MEMORY_SIZE; i++) {
        // If we find a bad block, we skip it and reset the starting pointer
        if (large_memory[i] == BAD_BLOCK) {
            start = &large_memory[i+1];
            address_count = 0;
            // printf("BAD BLOCK FOUND\n");
        }
        // If we still have not found the right number of memory addresses, increment the counter and continue searching
        else if (address_count != size) {
            address_count++;
        }
        // Otherwise, we have found the correct number of addresses, and we can return the pointer to the beginning of the continuous memory
        else {
            printf("Returning address %p\n", start);
            return start;
        }
    }





    // 1 & 2
    // char** allocated_memory = new char*[size];
    // Block* head = freeList;
    // // int block_count {0}; // tracker for how many good blocks have been allocated, if block_count exceeds the size the user input, return allocated_memory

    // // 3
    // // while (head->next != NULL) {
        
    // // }
    //         //     if (block_count >= size) {
    //         //     return allocated_memory;
    //         // }


    // for (size_t i = 0; i < LARGE_MEMORY_SIZE; i++) {
    //     if (large_memory[i] != BAD_BLOCK) {
    //         allocated_memory[i] = &large_memory[i];
    //     }
    // }
    
    
    

    
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

    // Mark some blocks as "bad"
    markBadBlocks(large_memory, LARGE_MEMORY_SIZE, 1000); // Mark 1000 bad blocks

    // Simulate memory allocation
    int *array = (int*)myMalloc(allocationSize * sizeof(int));  // Allocate memory for an array of integers
    printf("Address of array: %p\n", array);
    if (array == NULL) {
        printf("Memory allocation failed.\n");
    } else {
        // Assign values to the array and print them
        for (size_t i = 0; i < allocationSize; i++) {
            array[i] = i * i;  // Assign square of index
            printf("Array[%llu] = %d\t\t%p\n", i, array[i], &array[i]);
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