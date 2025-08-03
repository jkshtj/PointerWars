// MIT License. Copyright (c) 2025 Kshitij Jain
// See LICENSE file in the root of this repository.

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "bump_ptr_allocator.h"

#define FAIL(msg) printf("    FAIL! "); printf(#msg "\n"); fflush(stdout);

char* align_to(char* ptr, size_t alignment) {
  // assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "alignment must be a power of 2.");
  uintptr_t raw = (uintptr_t)ptr;
  uintptr_t aligned = (raw + alignment - 1) & ~(alignment - 1);
  return (char*)aligned;
}

bool slab_init(struct slab* slab, size_t alloc_size) {
  slab->data = (char*)malloc(alloc_size);
  if (slab->data == NULL) {
      FAIL("Failed to allocate memory for new slab!");
      return false;
  }
  
  slab->curr = slab->data;
  slab->end = slab->data + alloc_size;
  slab->alloc_size = alloc_size;
  return true;
}

bool slab_destroy(struct slab* slab) {
  if (slab == NULL) {
      return false;
  }

  free(slab->data);
  slab->curr = NULL;
  slab->end = NULL;
  slab->alloc_size = 0;

  return true;
}

char* slab_malloc(struct slab* slab, size_t size) {
  size_t alignment = 8; //align_of(type);
  char* new_alloc = align_to(slab->curr, alignment);
  char* new_alloc_end = new_alloc + size;

  if (new_alloc_end > slab->end) {
      return NULL;
  }

  slab->curr = new_alloc_end;

  return new_alloc;
}

bool bump_ptr_allocator_init(struct bump_ptr_allocator* allocator) {
  if (allocator->initialized) {
      FAIL("Allocator instance cannot be intialized multiple times.");
      exit(-1);
  }

  allocator->initialized = true;
  allocator->slab_ptr = 0;
  allocator->last_alloc_size = DEFAULT_ALLOC_SIZE_BYTES;
  allocator->total_mem_allocated = allocator->last_alloc_size;
  
  if (!slab_init(&allocator->slabs[allocator->slab_ptr], allocator->last_alloc_size)) {
      FAIL("Allocator failed to allocate memory for its first slab!");
      return false;
  }
  
  return true;
}

bool bump_ptr_allocator_destroy(struct bump_ptr_allocator* allocator) {
  if (allocator == NULL) {
      FAIL("Allocator instance cannot be NULL");
      exit(-1);
  }

  for (int i = 0; i < DEFAULT_NUM_SLABS; i++) {
      slab_destroy(&allocator->slabs[i]);
  }
  
  allocator->slab_ptr = 0;
  allocator->last_alloc_size = 0;
  allocator->total_mem_allocated = 0;
  allocator->initialized = false;

  return true;
}

void* bump_ptr_allocator_malloc(struct bump_ptr_allocator* allocator, size_t size) {
  if (allocator == NULL) {
      FAIL("Allocator instance cannot be NULL");
      exit(-1);
  }
  
  char* alloc = slab_malloc(&allocator->slabs[allocator->slab_ptr], size);

  if (alloc != NULL) {
      return alloc;
  }
  
  if (allocator->slab_ptr+1 >= DEFAULT_NUM_SLABS) {
      FAIL("Allocator ran out of memory to allocate!");
      return NULL;
  }

  allocator->slab_ptr += 1;
  allocator->last_alloc_size *= 2;
  allocator->total_mem_allocated += allocator->last_alloc_size;

  if (!slab_init(&allocator->slabs[allocator->slab_ptr], allocator->last_alloc_size)) {
      FAIL("Allocator failed to allocate memory for a new slab!");
      return NULL;
  }

  alloc = slab_malloc(&allocator->slabs[allocator->slab_ptr], size);
  if (alloc != NULL) {
      return (void*)allocator->slabs[allocator->slab_ptr].curr;
  }

  return NULL;
}

void* custom_malloc(size_t size) {
  return bump_ptr_allocator_malloc(&allocator, size);
}

void custom_free(void* addr) {
  (void)addr;
}

void bump_ptr_setup() {
  if (!allocator.initialized) {
      bump_ptr_allocator_init(&allocator);
  }
}

void bump_ptr_cleanup() {
  if (allocator.initialized) {
      bump_ptr_allocator_destroy(&allocator);
  }
}