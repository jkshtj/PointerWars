// MIT License. Copyright (c) 2025 Kshitij Jain
// See LICENSE file in the root of this repository.

#include "linked_list.h"

#define DEFAULT_ALLOC_SIZE_BYTES 4096
#define DEFAULT_NUM_SLABS 512

/**
 * Aligns a pointer to the specified alignment boundary.
 * \param ptr : Pointer to align
 * \param alignment : Alignment boundary (must be power of 2)
 * Returns aligned pointer, NULL on failure
 */
char* align_to(char* ptr, size_t alignment);

/**
 * Represents a single memory slab in the bump pointer allocator.
 * A slab is a contiguous block of memory from which allocations are made
 * by simply advancing a pointer (bump allocation).
 */
struct slab {
    char* data;        /* Pointer to the start of the slab's memory */
    char* curr;        /* Current allocation pointer within the slab */
    char* end;         /* Pointer to the end of the slab's memory */
    size_t alloc_size; /* Total size of this slab in bytes */
};

/**
 * Initializes a slab with the specified allocation size.
 * \param slab : Pointer to slab structure to initialize
 * \param alloc_size : Size in bytes to allocate for this slab
 * Returns true on success, false on failure
 */
bool slab_init(struct slab* slab, size_t alloc_size);

/**
 * Destroys a slab and frees its memory.
 * \param slab : Pointer to slab structure to destroy
 * Returns true on success, false on failure
 */
bool slab_destroy(struct slab* slab);

/**
 * Allocates memory from a slab using bump pointer allocation.
 * \param slab : Pointer to slab to allocate from
 * \param size : Number of bytes to allocate
 * Returns pointer to allocated memory on success, NULL on failure
 */
char* slab_malloc(struct slab* slab, size_t size);

/**
 * Bump pointer allocator structure that manages multiple slabs.
 * This allocator provides fast allocation by maintaining a collection
 * of slabs and allocating from them using bump pointer technique.
 * Memory is never freed individually - only when the entire allocator
 * is destroyed.
 */
struct bump_ptr_allocator {
    struct slab slabs[DEFAULT_NUM_SLABS]; /* Array of memory slabs */
    size_t slab_ptr;                      /* Index of current active slab */
    size_t last_alloc_size;               /* Size of the last allocation made */
    size_t total_mem_allocated;           /* Total memory allocated across all slabs */
    bool initialized;                     /* Whether the allocator has been initialized */
};

/**
 * Initializes a bump pointer allocator.
 * \param allocator : Pointer to allocator structure to initialize
 * Returns true on success, false on failure
 */
bool bump_ptr_allocator_init(struct bump_ptr_allocator* allocator);

/**
 * Destroys a bump pointer allocator and frees all its memory.
 * \param allocator : Pointer to allocator structure to destroy
 * Returns true on success, false on failure
 */
bool bump_ptr_allocator_destroy(struct bump_ptr_allocator* allocator);

/**
 * Allocates memory from the bump pointer allocator.
 * If the current slab doesn't have enough space, a new slab is created.
 * \param allocator : Pointer to allocator to allocate from
 * \param size : Number of bytes to allocate
 * Returns pointer to allocated memory on success, NULL on failure
 */
void* bump_ptr_allocator_malloc(struct bump_ptr_allocator* allocator, size_t size);

/**
 * Global bump pointer allocator instance used by the custom malloc/free functions.
 */
static struct bump_ptr_allocator allocator;

/**
 * Custom malloc implementation using the bump pointer allocator.
 * This function provides a malloc-compatible interface to the bump pointer allocator.
 * \param size : Number of bytes to allocate
 * Returns pointer to allocated memory on success, NULL on failure
 */
void* custom_malloc(size_t size);

/**
 * Custom free implementation for the bump pointer allocator.
 * Note: This is a no-op since bump pointer allocators don't support
 * individual memory deallocation. Memory is only freed when the entire
 * allocator is destroyed.
 * \param addr : Pointer to memory to free (ignored)
 */
void custom_free(void* addr);

/**
 * Sets up the global bump pointer allocator.
 * This function initializes the global allocator instance and should be
 * called before using custom_malloc/custom_free functions.
 */
void bump_ptr_setup();

/**
 * Cleans up the global bump pointer allocator.
 * This function destroys the global allocator instance and frees all
 * allocated memory. Should be called when done using the allocator.
 */
void bump_ptr_cleanup();
