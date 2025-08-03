// MIT License. Copyright (c) 2025 Kshitij Jain
// See LICENSE file in the root of this repository.

#include <assert.h>
#include <stdio.h>

#include "bump_ptr_allocator.h"
#include "linked_list.h"

// Function pointers to (potentially) custom malloc() and
// free() functions.
//
static void * (*malloc_fptr)(size_t size) = NULL;
static void   (*free_fptr)(void* addr)    = NULL; 

// (void)(malloc_fptr);
// (void)(free_fptr);

// Creates a new linked_list.
// PRECONDITION: Register malloc() and free() functions via the
//               linked_list_register_malloc() and 
//               linked_list_register_free() functions.
// POSTCONDITION: An empty linked_list has its head point to NULL.
// Returns a new linked_list on success, NULL on failure.
//
struct linked_list * linked_list_create() {
    struct linked_list* empty_list = (struct linked_list*)malloc_fptr(sizeof(struct linked_list));
    return empty_list != NULL ? 
        (empty_list->head = NULL, empty_list->tail = NULL, empty_list->size = 0, empty_list) : 
        empty_list;
}

// Deletes a linked_list and frees all memory assoicated with it.
// \param ll : Pointer to linked_list to delete
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_delete(struct linked_list * ll) {
    if (ll == NULL) {
        return false;
    }

    struct node* curr = ll->head;
    while (curr != NULL) {
        struct node* next = curr->next;
        free_fptr(curr);
        curr = next;
    }

    free_fptr(ll);
    return true;
}

// Returns the size of a linked_list.
// \param ll : Pointer to linked_list.
// Returns size on success, SIZE_MAX on failure.
//
size_t linked_list_size(struct linked_list * ll) {
    if (ll == NULL) {
        return SIZE_MAX;
    }

    return ll->size;
}

// Inserts an element at the end of the linked_list.
// \param ll   : Pointer to linked_list.
// \param data : Data to insert.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_insert_end(struct linked_list * ll,
                            unsigned int data) 
{
    return ll != NULL ? linked_list_insert(ll, linked_list_size(ll), data) : false;
}

// Inserts an element at the front of the linked_list.
// \param ll   : Pointer to linked_list.
// \param data : Data to insert.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_insert_front(struct linked_list * ll,
                              unsigned int data) 
{
    return linked_list_insert(ll, 0, data);
}

// Internal utility function to initialize an already 
// allocated iterator.
// \param iter        : Allocated iterator.  
// \param linked_list : Pointer to linked_list.
// \param index       : Index of the linked list to start at.
//
static void _linked_list_init_iterator(struct iterator* iter, struct linked_list* ll, size_t index) {
    assert(iter != NULL && "Iterator to be populated must not be NULL!");

    struct node* curr = ll->head;
    size_t i = index;
    while (i-- > 0) {
        curr = curr->next;
    }

    iter->ll = ll;
    iter->current_index = index;
    iter->current_node = curr;
    iter->data = curr->data;
}

// Inserts an element at a specified index in the linked_list.
// \param ll    : Pointer to linked_list.
// \param index : Index to insert data at.
// \param data  : Data to insert.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_insert(struct linked_list* ll,
                        size_t index,
                        unsigned int data) 
{
    size_t ll_size = linked_list_size(ll);
    if (ll == NULL || ll_size < index) {
        return false;
    }

    struct node* new_node = (struct node*)malloc_fptr(sizeof(struct node));
    new_node->data = data;

    if (index == 0) {
        new_node->next = ll->head;
        ll->head = new_node;
        if (ll_size == 0) ll->tail = ll->head;
    } else if (index == ll_size) {
        ll->tail->next = new_node;
        ll->tail = new_node;
    } else {
        struct iterator iter;
        _linked_list_init_iterator(&iter, ll, index-1);
        
        struct node* prev = iter.current_node;
        new_node->next = prev->next;
        prev->next = new_node;
    }

    ll->size += 1;
    return true;
}

// Finds the first occurrence of data and returns its index.
// \param ll   : Pointer to linked_list.
// \param data : Data to find.
// Returns index of the first index with that data, SIZE_MAX otherwise.
//
size_t linked_list_find(struct linked_list * ll,
                        unsigned int data) 
{
    if (ll == NULL) {
        return SIZE_MAX;
    }

    size_t result = SIZE_MAX;
    struct node* curr = ll->head;
    size_t i = 0;

    while (curr != NULL) {
        if (curr->data == data) {
            result = i;
            break;
        }
        curr = curr->next;
        i += 1;
    }

    return result;
}

// Removes a node from the linked_list at a specific index.
// \param ll    : Pointer to linked_list.
// \param index : Index to remove node.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_remove(struct linked_list * ll,
                        size_t index) 
{
    if (ll == NULL || linked_list_size(ll) <= index) {
        return false;
    }

    if (index == 0) {
        struct node* to_remove = ll->head;
        ll->head = ll->head->next;
        free_fptr(to_remove);
    } else {
        struct iterator iter;
        _linked_list_init_iterator(&iter, ll, index-1);

        struct node* prev = iter.current_node;
        struct node* to_remove = prev->next;
        prev->next = to_remove->next;
        if (ll->tail == to_remove) {
            ll->tail = prev;
        }
        free_fptr(to_remove);
    }

    ll->size -=1 ;
    return true;
}

// Creates an iterator struct at a particular index.
// \param linked_list : Pointer to linked_list.
// \param index       : Index of the linked list to start at.
// Returns pointer to an iterator on success, NULL otherwise.
//
struct iterator * linked_list_create_iterator(struct linked_list * ll,
                                              size_t index)
{
    if (ll == NULL || linked_list_size(ll) <= index) {
        return NULL;
    }

    struct iterator* iter = (struct iterator*)malloc_fptr(sizeof(struct iterator));
    _linked_list_init_iterator(iter, ll, index);
    return iter;
}

// Deletes an iterator struct.
// \param iterator : Iterator to delete.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_delete_iterator(struct iterator * iter) {
    if (iter == NULL) {
        return false;
    }
    free_fptr(iter);
    return true;
}

// Iterates to the next node in the linked_list.
// \param iterator: Iterator to iterate on.
// Returns TRUE when next node is present, FALSE once end of list is reached.
//
bool linked_list_iterate(struct iterator * iter) {
    if (iter == NULL) {
        return false;
    }

    if (iter->current_node->next != NULL) {
        iter->current_node = iter->current_node->next;
        iter->current_index += 1;
        iter->data = iter->current_node->data;
        return true;
    }

    return false;
}

// Registers malloc() function.
// \param malloc : Function pointer to malloc()-like function.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_register_malloc(void * (*malloc)(size_t)) {
    malloc_fptr = malloc;
    printf("Registered custom malloc function in linked_list: %p\n", malloc_fptr);
    return true;
}

// Registers free() function.
// \param free : Function pointer to free()-like function.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_register_free(void (*free)(void*)) {
    free_fptr = free;
    printf("Registered custom free function in linked_list: %p\n", free_fptr);
    return true;
}