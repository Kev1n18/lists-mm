#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "list_mm.h"
#include "memory_manager.h"

struct Node_ {
    Element element;
    FileCell next;
};
typedef struct Node_ Node_, *Node;

typedef struct {
    FileCell head;
    FileCell tail;
    size_t size;
} ListMMIndex;

struct ListMM_ {
    FileMem file_mem;
    ListMMIndex index;
};

// Creates a new list.
ListMM list_create(const char* file_name) {
    ListMM list = malloc(sizeof(struct ListMM_));
    list->file_mem = create_file(file_name, sizeof(ListMMIndex), sizeof(struct Node_));
    if (list->file_mem == NULL) {
        free(list);
        list = NULL;
    } else {
        list->index.head = NULL_CELL;
        list->index.tail = NULL_CELL;
        list->index.size = 0;
    }
    return list;
}

// Destroys a list.
void list_destroy(ListMM list) {
    close_file(list->file_mem);
    free(list);
}

// Opens a list.
ListMM list_open(const char* file_name) {
    ListMM list = malloc(sizeof(struct ListMM_));
    list->file_mem = open_file(file_name);
    if (list->file_mem == NULL) {
        free(list);
        return NULL;
    } else {
        read_index(list->file_mem, (void*)&(list->index));
        return list;
    }
}

// Closes a list;
void list_close(ListMM list) {
    write_index(list->file_mem, (void*)&(list->index));
    close_file(list->file_mem);
    free(list);
}

// Returns true iff the list contains no elements.
bool list_is_empty(ListMM list) {
    return list->index.size == 0;
}

// Returns the number of elements in the list.
size_t list_size(ListMM list) {
    return list->index.size;
}

// Inserts the specified element at the first position in the list.
void list_insert_first(ListMM list, Element* element) {
    Node_ node;
    memcpy(&node.element, element, sizeof(Element));
    FileCell cell = new_cell(list->file_mem);
    if (cell != NULL_CELL) {
        node.next = list->index.head;
        list->index.head = cell;
        if (list_is_empty(list)) {
            list->index.tail = cell;
        }
        list->index.size++;
        write_cell(list->file_mem, cell, (void*)&node);
        write_index(list->file_mem, (void*)&(list->index));
    }
}

// Inserts the specified element at the last position in the list.
void list_insert_last(ListMM list, Element* element) {
    Node_ node;
    memcpy(&node.element, element, sizeof(Element));
    FileCell cell = new_cell(list->file_mem);
    if (cell != NULL_CELL) {
        node.next = NULL_CELL;
        write_cell(list->file_mem, cell, (void*)&node);
        if (list_is_empty(list)) {
            list->index.head = cell;
        } else {
            Node_ prev_node;
            FileCell prev_cell = list->index.tail;
            read_cell(list->file_mem, prev_cell, (void*)&prev_node);
            prev_node.next = cell;
            write_cell(list->file_mem, prev_cell, (void*)&prev_node);
        }
        list->index.tail = cell;
        list->index.size++;
        write_index(list->file_mem, (void*)&(list->index));
    }
}

// Inserts the specified element at the specified position in the list.
// Range of valid positions: 0, ..., size().
// If the specified position is 0, insert corresponds to insertFirst.
// If the specified position is size(), insert corresponds to insertLast.
void list_insert(ListMM list, Element* element, size_t position) {
    if (position == 0) {
        list_insert_first(list, element);
    } else if (position == list_size(list)) {
        list_insert_last(list, element);
    } else {
        Node_ node;
        memcpy(&node.element, element, sizeof(Element));
        FileCell cell = new_cell(list->file_mem);
        if (cell != NULL_CELL) {
            Node_ prev_node;
            FileCell prev_cell = list->index.head;
            read_cell(list->file_mem, prev_cell, (void*)&prev_node);
            for (size_t i = 0; i < position - 1; i++) {
                prev_cell = prev_node.next;
                read_cell(list->file_mem, prev_cell, (void*)&prev_node);
            }
            node.next = prev_node.next;
            prev_node.next = cell;
            write_cell(list->file_mem, prev_cell, (void*)&prev_node);
            write_cell(list->file_mem, cell, (void*)&node);
            list->index.size++;
            write_index(list->file_mem, (void*)&(list->index));
        }
    }
}

// Returns the position in the list of the
// first occurrence of the specified element,
// or -1 if the specified element does not
// occur in the list.
int list_find(ListMM list, Element* element) {
    int position = 0;
    FileCell cell = list->index.head;
    Node_ node;
    read_cell(list->file_mem, cell, (void*)&node);
    while (cell != NULL_CELL) {
        if (memcmp(&node.element, element, sizeof(Element)) != 0) {
            cell = node.next;
            read_cell(list->file_mem, cell, (void*)&node);
            position++;
        } else {
            return position;
        }
    }
    return -1;
}

// Returns the first element of the list.
Element list_get_first(ListMM list) {
    Node_ node;
    if (!list_is_empty(list)) {
        FileCell cell = list->index.head;
        read_cell(list->file_mem, cell, (void*)&node);
    }
    return node.element;
}

// Returns the last element of the list.
Element list_get_last(ListMM list) {
    Node_ node;
    if (!list_is_empty(list)) {
        FileCell cell = list->index.tail;
        read_cell(list->file_mem, cell, (void*)&node);
    }
    return node.element;
}

// Returns the element at the specified position in the list.
// Range of valid positions: 0, ..., size()-1.
Element list_get(ListMM list, size_t position) {
    Node_ node;
    if (position == 0) {
        return list_get_first(list);
    } else if (position == list_size(list) - 1) {
        return list_get_last(list);
    } else if (position < list_size(list)) {
        FileCell cell = list->index.head;
        read_cell(list->file_mem, cell, (void*)&node);
        for (size_t i = 0; i < position; i++) {
            cell = node.next;
            read_cell(list->file_mem, cell, (void*)&node);
        }
    }
    return node.element;
}

// Removes and returns the element at the first position in the list.
Element list_remove_first(ListMM list) {
    Node_ node;
    Element element;
    if (!list_is_empty(list)) {
        FileCell cell = list->index.head;
        read_cell(list->file_mem, cell, (void*)&node);
        element = node.element;

        list->index.head = node.next;
        list->index.size--;
        write_index(list->file_mem, (void*)&(list->index));
        
        free_cell(list->file_mem, cell);
    }
    return element;
}

// Removes and returns the element at the last position in the list.
Element list_remove_last(ListMM list) {
    Node_ tail, prev_node;
    Element element;
    if (!list_is_empty(list)) {
        FileCell prev_cell = list->index.head;
        read_cell(list->file_mem, prev_cell, (void*)&prev_node);
        while (prev_node.next != list->index.tail) {
            prev_cell = prev_node.next;
            read_cell(list->file_mem, prev_cell, (void*)&prev_node);
        }

        FileCell tail_cell = prev_node.next;
        read_cell(list->file_mem, tail_cell, (void*)&tail);
        element = tail.element;

        prev_node.next = NULL_CELL;
        write_cell(list->file_mem, prev_cell, (void*)&prev_node);

        list->index.tail = prev_cell;
        list->index.size--;
        write_index(list->file_mem, (void*)&(list->index));

        free_cell(list->file_mem, tail_cell);
    }
    return element;
}

// Removes and returns the element at the specified position in the list.
// Range of valid positions: 0, ..., size()-1.
Element list_remove(ListMM list, size_t position) {
    Node_ node, prev_node;
    Element element;
    if (position == 0) {
        return list_remove_first(list);
    } else if (position == list_size(list) - 1) {
        return list_remove_last(list);
    } else if (position < list_size(list)) {
        FileCell prev_cell = list->index.head;
        read_cell(list->file_mem, prev_cell, (void*)&prev_node);
        for (size_t i = 0; i < position - 1; i++) {
            prev_cell = prev_node.next;
            read_cell(list->file_mem, prev_cell, (void*)&prev_node);
        }
        FileCell cell = prev_node.next;
        read_cell(list->file_mem, cell, (void*)&node);
        element = node.element;

        prev_node.next = node.next;
        write_cell(list->file_mem, prev_cell, (void*)&prev_node);
        
        list->index.size--;
        write_index(list->file_mem, (void*)&(list->index));
        free_cell(list->file_mem, cell);
    }
    return element;
}

// Removes all elements from the list.
void list_make_empty(ListMM list) {
    Node_ node;
    FileCell cell = list->index.head;
    read_cell(list->file_mem, cell, (void*)&node);
    while (cell != NULL_CELL) {
        cell = node.next;
        read_cell(list->file_mem, cell, (void*)&node);
        free_cell(list->file_mem, cell);
    }
    list->index.head = NULL_CELL;
    list->index.tail = NULL_CELL;
    list->index.size = 0;
    write_index(list->file_mem, (void*)&(list->index));
}