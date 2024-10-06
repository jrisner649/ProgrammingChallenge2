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
#define LARGE_MEMORY_SIZE (100) // 1 MB
#define BAD_BLOCK 'X'

// Simulate the memory space for our allocator
static char memory[MEMORY_SIZE];

// Simulate a large memory space for testing
char *large_memory = NULL;

// Keep track of the addresses each memory block is mapped to
char *memory_map[LARGE_MEMORY_SIZE];

static int block_count {0};

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

void displayLinkedList(Block* head) {
    Block* temp = head;
    size_t i {0};
    size_t sum {0};
    printf("\n");
    while (temp != NULL) {
        printf("Address of block #%llu: %p\n", i, temp);
        printf("Size of block #%llu: %llu\n", i, temp->size);
        printf("Total size of block #%llu: %llu\n", i, temp->size+sizeof(Block));
        sum += temp->size+sizeof(Block);
        printf("\n");
        i++;
        temp = temp->next;
    }
    printf("Total memory used: %llu\n", sum);
}

// Skeleton function: Allocate memory dynamically, skipping bad blocks
void* myMalloc(size_t size) {
    // STUDENTS: Implement logic to allocate memory dynamically, ensuring that you skip over bad blocks

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

                    // Map the block to the memory addresses it is using. We will need this information when we free the block later
                    block_count++;
                    memory_map[block_count] = &large_memory[i-good_count];

                    allocated_block->size = size;   // Match the size of the allocated block to the size of the data to be allocated
                    allocated_block->free = false;  // The block is not free since we are allocating it

                    // Insert the allocated block into the list
                    Block* temp = freeList->next;
                    freeList->next = allocated_block;
                    allocated_block->next = temp;

                    char* memory_ptr = &memory[freeList->size + 2*sizeof(Block)];
                    return memory_ptr;
                }
            }
    }

    return NULL;    // Will return NULL if we did not have enough free space in the memory array, or there was not enough contiguous good memory in the large memory array
}

// Skeleton function: Free the allocated memory
void myFree(void *ptr) {
    // STUDENTS: Implement logic to free the allocated memory
}

void testArray(int* array, size_t allocationSize) {
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
}

void testArray(char* array, size_t allocationSize) {
    if (array == NULL) {
        printf("Memory allocation failed.\n");
    } else {
        // Assign values to the array and print them
        char letter = 'a';
        // size_t hex = 0x0;
        for (size_t i = 0; i < allocationSize; i++) {
            array[i] = letter + i;  // Assign square of index
            printf("Array[%llu] = %c\t\t%p\n", i, array[i], &array[i]);
        }

        // Free the allocated memory
        myFree(array);
        printf("Memory successfully freed.\n");
    }
}

void testArray(double* array, size_t allocationSize) {
    if (array == NULL) {
        printf("Memory allocation failed.\n");
    } else {
        // Assign values to the array and print them
        for (size_t i = 0; i < allocationSize; i++) {
            array[i] = i * i;  // Assign square of index
            printf("Array[%llu] = %f\t\t%p\n", i, array[i], &array[i]);
        }

        // Free the allocated memory
        myFree(array);
        printf("Memory successfully freed.\n");
    }
}

void displayLargeMemory() {
    size_t bad_count {0};
    for (size_t i {0}; i < LARGE_MEMORY_SIZE; i++) {
        printf("large_memory[%llu] = %c\n", i, large_memory[i]);
        if (large_memory[i] == BAD_BLOCK)
            bad_count++;
    }
    printf("Number of bad blocks: %llu\n", bad_count);
}

void displayMemoryMap() {
    for (size_t i {0}; i < LARGE_MEMORY_SIZE; i++) {
        printf("memory_map[%llu] = %p\n", i, memory_map[i]);
    }
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
    markBadBlocks(large_memory, LARGE_MEMORY_SIZE, 10); // Mark 1000 bad blocks

    // Simulate memory allocation
    // int *array = (int*)myMalloc(allocationSize * sizeof(int));  // Allocate memory for an array of integers
    char *array2 = (char*)myMalloc(allocationSize * sizeof(char));
    // double *array3 = (double*)myMalloc(allocationSize * sizeof(double));

    // testArray(array, allocationSize);
    testArray(array2, allocationSize);
    // testArray(array3, allocationSize);

    displayLargeMemory();

    displayMemoryMap();

    displayLinkedList(freeList);

    // Clean up large memory block using system's free function
    myFree(large_memory);

    // REMEBER TO REMOVE THIS AFTER myFree() HAS BEEN COMPLETED
    delete[] array2;
    free(large_memory);

    return 0;
}