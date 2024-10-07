/*
    Title: Programming Challenge 2
    Authors: Seth Risner and Connor Smith
    Description: This program simulates dynamic memory allocation
    Date: 7 Oct. 2024
*/

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
    freeList->size = MEMORY_SIZE - sizeof(Block);
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

    // We must be allocating at least 1 byte. Fail by returning NULL if this is not the case
    if (size <= 0) {
        return NULL;
    }

    // Traverse the freeList and search for a suitable block
    Block* head = freeList;
    bool good_block_found {false};
    while (head != NULL) {

        // If we find a block that is large enough and free, then we have found a good block
        if (head->size >= (size + sizeof(Block)) && head->free == true) {
            good_block_found = true;
            break;
        }

        head = head->next;
    }

    // If we found a good block, search the large memory space for contiguous good data
    if (good_block_found == true) {
            size_t good_count {0};
            for (size_t i {0}; i < LARGE_MEMORY_SIZE; i++) {

                // We have found a bad byte or a byte that has already been allocated
                if (large_memory[i] == BAD_BLOCK || large_memory[i] == '1') {
                    good_count = 0;     // reset the counter for the good data we have found
                }

                // If we still do not have enough bytes, continue searching
                else if (good_count != size) {
                    good_count++;
                }

                // Else, we have found a large enough contiguous memory space with good data
                else {

                    // Simulate addresses being marked as 'not free' by assigning the '1' character
                    for (size_t j {i-good_count}; j < i; j++) {
                        large_memory[j] = '1';
                    }

                    freeList->size = (freeList->size) - (size) - (sizeof(Block));    // Adjust the amount of free memory that is available by subtracting the size of the block we just allocated
                    Block* allocated_block = (Block*)&memory[sizeof(Block) + freeList->size];  // Store a new block in memory

                    allocated_block->size = size;   // Match the size of the allocated block to the size of the data to be allocated
                    allocated_block->free = false;  // The block is not free since we are allocating it

                    // Insert the allocated block into the list
                    Block* temp = freeList->next;
                    freeList->next = allocated_block;
                    allocated_block->next = temp;

                    char* memory_ptr = &memory[freeList->size + 2*sizeof(Block)];   // return the pointer to the beginning of the data we just allocated
                    return memory_ptr;                                              // 2*sizeof(Block) comes from the fact that we have to pass over the freeList Block and the allocated Block we just created
                }                                                                   // freeList->size because we have to pass over the data that the freeList Block is managing
            }
    }

    return NULL;    // Will return NULL if we did not have enough free space in the memory array, or there was not enough contiguous good memory in the large memory array
}

// Skeleton function: Free the allocated memory
void myFree(void *ptr) {
    // STUDENTS: Implement logic to free the allocated memory

    // If the pointer is NULL, then there is nothing to free, so simply return
    if (ptr == NULL) {
        return;
    }

    // The pointer that is passed into myFree points to the beginning of the data allocated by a block.
    // Therefore, in order to free the block, we must move backwards to the beginning of the metadata contained by the Block struct.
    ptr = ptr-sizeof(Block);  
    Block* block_ptr = (Block*)ptr; // Once we move backwards, make the pointer a pointer to a Block.

    // If the block is not free, make it free
    if (block_ptr->free == false) {
        block_ptr->free = true;
    }
    // Otherwise, the block is already free, so simply return
    else {
        return;
    }

    // Now combine the free blocks
    // First, find the block located just before the block we are freeing in the linked list
    Block* head = freeList;
    while (head->next != block_ptr) {
        head = head->next;
    };

    // Merge adjacent blocks that are free to mitigate memory fragmentation
    size_t free_sum {0};
    // If head->free (if the previous block is free), then we will merge it
    if (head->free == true) {
        free_sum = head->size + block_ptr->size + sizeof(Block);    // The amount of free data is the sum of the sizes of the blocks, and additionally sizeof(Block) since we are merging the two together
        head->size = free_sum;          // By setting the size of head to be the free_sum
        head->next = block_ptr->next;   // and changing head's next pointer, the Block pointed to by block_ptr is merged into head.

        // Next, we check if the next block in the list is also free and perform the same process as above
        if (block_ptr->next != NULL && block_ptr->next->free == true) {
            free_sum = block_ptr->next->size + sizeof(Block);
            head->size += free_sum;
            if (block_ptr->next->next != NULL) {
                head->next = block_ptr->next->next;
            }
            else {
                head->next = NULL;
            }
        }
    }
    // If the previous block is not free, check if the next block is free
    else if (block_ptr->next != NULL && block_ptr->next->free == true) {
        free_sum = block_ptr->next->size + sizeof(Block);   // Merge the blocks by combining their sizes 
        block_ptr->size += free_sum;                        // and adjusting the next pointers in the linked list
        if (block_ptr->next->next != NULL) {
            block_ptr->next = block_ptr->next->next;
        }
        else {
            block_ptr->next = NULL;
        }
        
    }    
    
}

// Unit Test Functions
void testMemoryAllocation() {
    // Test 1: Allocate memory for an array of size 10
    int *array = (int*)myMalloc(10 * sizeof(int));
    if (array != NULL) {
        printf("Test 1 Passed: Memory allocated for an array of size 10\n");
        myFree(array);
    } else {
        printf("Test 1 Failed: Memory allocation for array of size 10 failed.\n");
    }

    // Test 2: Try to allocate more memory than available
    int *largeArray = (int*)myMalloc(MEMORY_SIZE + 1);
    if (largeArray == NULL) {
        printf("Test 2 Passed: Large memory block allocation failed as expected.\n");
    } else {
        printf("Test 2 Failed: Large memory block should not have been allocated.\n");
        myFree(largeArray);
    }

    // Test 3: Allocate memory while skipping bad blocks
    int *testBlock = (int*)myMalloc(50 * sizeof(int));
    if (testBlock != NULL) {
        printf("Test 3 Passed: Memory allocated by skipping bad blocks.\n");
        myFree(testBlock);
    } else {
        printf("Test 3 Failed: Memory allocation should have succeeded by skipping bad blocks.\n");
    }
}

// Run all tests
void runTests() {
    testMemoryAllocation();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <size_of_allocation>\n", argv[0]);
        return 1;
    }

    // Convert the command-line argument to an integer for allocation size
    size_t allocationSize = atoi(argv[1]);

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
    if (array == NULL) {
        printf("Memory allocation failed.\n");
    } else {
        // Assign values to the array and print them
        for (int i = 0; i < allocationSize; i++) {
            array[i] = i * i;  // Assign square of index
            printf("Array[%d] = %d\n", i, array[i]);
        }

        // Free the allocated memory
        myFree(array);
        printf("Memory successfully freed.\n");
    }

    runTests();

    // Clean up large memory block using system's free function
    myFree(large_memory);

    return 0;
}