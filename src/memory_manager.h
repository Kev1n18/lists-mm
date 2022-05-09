#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdio.h>

typedef int FileCell;
#define NULL_CELL 0

typedef struct _FileMem *FileMem;

// Creates and opens a file whose name is the string pointed to by fileName, if
// the file does not exist, otherwise returns NULL. The index has index_size
// bytes, and cells have cell_size bytes. Pre-condition: theCellSize >= 4.
FileMem create_file(const char *file_name, int index_size, int cell_size);

// Opens the file whose name is the string pointed to by file_name, if the file
// exists, otherwise, returns NULL.
FileMem open_file(const char *file_name);

// Closes the specified file.
void close_file(FileMem file_mem);

// Reads the index from the specified file, storing it at the location given by
// index.
void read_index(FileMem file_mem, void *idx);

// Writes the index to the specified file, obtaining it from the location given
// by index.
void write_index(FileMem file_mem, void *idx);

// Reads from the specified file the cell whose reference is file_cell, storing
// it at the location given by cell.
void read_cell(FileMem file_mem, FileCell file_cell, void *cell);

// Writes to the specified file the cell whose reference is file_cell, obtaining
// it from the location given by cell.
void write_cell(FileMem file_mem, FileCell file_cell, void *cell);

// Allocates memory to a new cell in the specified file, and returns a reference
// to it.
FileCell new_cell(FileMem file_mem);

// Frees the memory previously allocated to the cell whose reference is
// file_cell in the specified file.
void free_cell(FileMem file_mem, FileCell cell);

#endif
