#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "memory_manager.h"

struct Data {
    int value;
    char id[8];
};
typedef struct Data Element;

typedef struct ListMM_* ListMM;

// Creates a new list.
ListMM list_create(const char* file_name);

// Destroys a list.
void list_destroy(ListMM list);

// Opens a list.
ListMM list_open(const char* file_name);

// Closes a list;
void list_close(ListMM list);

// Returns true iff the list contains no elements.
bool list_is_empty(ListMM list);

// Returns the number of elements in the list.
size_t list_size(ListMM list);

// Returns the first element of the list.
Element list_get_first(ListMM list);

// Returns the last element of the list.
Element list_get_last(ListMM list);

// Returns the element at the specified position in the list.
// Range of valid positions: 0, ..., size()-1.
Element list_get(ListMM list, size_t position);

// Returns the position in the list of the
// first occurrence of the specified element,
// or -1 if the specified element does not
// occur in the list.
int list_find(ListMM list, bool (*equal)(Element*, Element*), Element* element);

// Inserts the specified element at the first position in the list.
void list_insert_first(ListMM list, Element* element);

// Inserts the specified element at the last position in the list.
void list_insert_last(ListMM list, Element* element);

// Inserts the specified element at the specified position in the list.
// Range of valid positions: 0, ..., size().
// If the specified position is 0, insert corresponds to insertFirst.
// If the specified position is size(), insert corresponds to insertLast.
void list_insert(ListMM list, Element* element, size_t position);

// Removes and returns the element at the first position in the list.
Element list_remove_first(ListMM list);

// Removes and returns the element at the last position in the list.
Element list_remove_last(ListMM list);

// Removes and returns the element at the specified position in the list.
// Range of valid positions: 0, ..., size()-1.
Element list_remove(ListMM list, size_t position);

// Removes all elements from the list.
void list_make_empty(ListMM list);

#endif