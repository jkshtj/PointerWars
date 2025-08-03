// MIT License. Copyright (c) 2025 Kshitij Jain
// See LICENSE file in the root of this repository.

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Function pointers to (potentially) custom malloc() and
// free() functions.
//
static void * (*malloc_fptr)(size_t size) = NULL;
static void   (*free_fptr)(void* addr)    = NULL; 

// Creates a new queue.
// PRECONDITION: Register malloc() and free() functions via the
//               queue_register_malloc() and 
//               queue_register_free() functions.
// Returns a new linked_list on success, NULL on failure.
//
struct queue * queue_create(void) {
  struct queue* queue = (struct queue*)malloc_fptr(sizeof(struct queue));
  return queue != NULL
         ? (queue->ll = linked_list_create(), queue)
         : queue;
}

// Deletes a linked_list.
// \param queue : Pointer to queue to delete
// Returns TRUE on success, FALSE otherwise.
//
bool queue_delete(struct queue * queue) {
  if (queue == NULL) {
    return false;
  }

  linked_list_delete(queue->ll);
  free_fptr(queue);
  
  return true;
}

// Pushes an unsigned int onto the queue.
// \param queue : Pointer to queue.
// \param data  : Data to insert.
// Returns TRUE on success, FALSE otherwise.
//
bool queue_push(struct queue * queue, unsigned int data) {
  if (queue == NULL) {
    return false;
  }

  return linked_list_insert_end(queue->ll, data);
}

// Pops an unsigned int from the queue, if one exists.
// \param queue       : Pointer to queue.
// \param popped_data : Pointer to popped data (provided by caller), if pop occurs.
// Returns TRUE on success, FALSE otherwise.
//
bool queue_pop(struct queue * queue, unsigned int * popped_data) {
  if (queue == NULL) {
    return false;
  }

  if (!queue_next(queue, popped_data)) {
    return false;
  }
  
  return linked_list_remove(queue->ll, 0);
} 

// Returns the size of the queue.
// \param queue : Pointer to queue.
// Returns size on success, SIZE_MAX otherwise.
//
size_t queue_size(struct queue * queue) {
  if (queue == NULL) {
    return SIZE_MAX;
  }

  return linked_list_size(queue->ll);
}

// Returns whether an entry exists to be popped.
// \param queue: Pointer to queue.
// Returns TRUE if an entry can be popped, FALSE otherwise.
//
bool queue_has_next(struct queue * queue) {
  if (queue == NULL) {
    return false;
  }

  return queue_size(queue) > 0;
}

// Returns the value at the head of the queue, but does
// not pop it.
// \param queue       : Pointer to queue.
// \param popped_data : Pointer to popped data (provided by caller), if pop occurs.
// Returns TRUE on success, FALSE otherwise.
//
bool queue_next(struct queue * queue, unsigned int * popped_data) {
  if (!queue_has_next(queue)) {
    return false;
  }

  struct iterator* iter = linked_list_create_iterator(queue->ll, 0);
  if (iter == NULL) {
    return false;
  }

  *popped_data = iter->data;
  return true;
}


bool queue_register_malloc(void * (*malloc)(size_t)) {
  malloc_fptr = malloc;
  linked_list_register_malloc(malloc);
  return true;
}

bool queue_register_free(void (*free)(void*)) {
  free_fptr = free;
  linked_list_register_free(free);
  return true;
}
