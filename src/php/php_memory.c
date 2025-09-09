/**
 * PHP Memory Management
 * Memory allocation and garbage collection for WebAssembly
 */

#include "php_engine.h"
#include <stdlib.h>
#include <string.h>

// Memory pool for efficient allocation
typedef struct memory_block {
    void* ptr;
    size_t size;
    bool in_use;
    struct memory_block* next;
} memory_block_t;

static memory_block_t* memory_pool = NULL;
static size_t total_allocated = 0;
static size_t peak_allocated = 0;

// Initialize memory management
bool php_memory_init(void) {
    memory_pool = NULL;
    total_allocated = 0;
    peak_allocated = 0;
    return true;
}

// Cleanup memory management
void php_memory_cleanup(void) {
    memory_block_t* current = memory_pool;
    while (current) {
        memory_block_t* next = current->next;
        if (current->ptr) {
            free(current->ptr);
        }
        free(current);
        current = next;
    }
    memory_pool = NULL;
}

// Allocate memory
void* php_memory_alloc(size_t size) {
    memory_block_t* block = malloc(sizeof(memory_block_t));
    if (!block) return NULL;
    
    block->ptr = malloc(size);
    if (!block->ptr) {
        free(block);
        return NULL;
    }
    
    block->size = size;
    block->in_use = true;
    block->next = memory_pool;
    memory_pool = block;
    
    total_allocated += size;
    if (total_allocated > peak_allocated) {
        peak_allocated = total_allocated;
    }
    
    return block->ptr;
}

// Reallocate memory
void* php_memory_realloc(void* ptr, size_t new_size) {
    if (!ptr) return php_memory_alloc(new_size);
    
    // Find the block
    memory_block_t* current = memory_pool;
    while (current) {
        if (current->ptr == ptr) {
            void* new_ptr = realloc(ptr, new_size);
            if (new_ptr) {
                current->ptr = new_ptr;
                current->size = new_size;
                return new_ptr;
            }
            return NULL;
        }
        current = current->next;
    }
    
    return NULL;
}

// Free memory
void php_memory_free(void* ptr) {
    if (!ptr) return;
    
    memory_block_t* current = memory_pool;
    memory_block_t* prev = NULL;
    
    while (current) {
        if (current->ptr == ptr) {
            free(ptr);
            total_allocated -= current->size;
            
            if (prev) {
                prev->next = current->next;
            } else {
                memory_pool = current->next;
            }
            
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Get memory statistics
size_t php_memory_get_usage(void) {
    return total_allocated;
}

size_t php_memory_get_peak_usage(void) {
    return peak_allocated;
}
