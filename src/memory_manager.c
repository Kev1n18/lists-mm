#include "memory_manager.h"
#include <stdlib.h>

typedef struct {
    int index_size;
    int cell_size;
    int num_cells;
    FileCell free_cells;
} _ControlInfo;

struct _FileMem {
    _ControlInfo control_info;
    FILE *file;
};

#define FILE_CELL_SIZE sizeof(FileCell)
#define CONTROL_INFO_SIZE sizeof(_ControlInfo)

// Maps virtual cell references (1, 2, ...) into cell positions in the file.
long virtual_to_real(FileMem file_mem, FileCell file_cell) {
    return CONTROL_INFO_SIZE + file_mem->control_info.index_size + (file_cell - 1) * file_mem->control_info.cell_size;
}

// Reads the control_info from the file.
void readControlInfo(FileMem file_mem) {
    fseek(file_mem->file, 0L, SEEK_SET);
    fread((void *)&file_mem->control_info, CONTROL_INFO_SIZE, 1, file_mem->file);
}

// Writes the control_info to the file.
void write_control_info(FileMem file_mem) {
    fseek(file_mem->file, 0L, SEEK_SET);
    fwrite((void *)&file_mem->control_info, CONTROL_INFO_SIZE, 1, file_mem->file);
}

// Reads from the specified file the cell reference stored in the cell whose
// reference is file_cell, and returns it.
FileCell get_next_file_cell(FileMem file_mem, FileCell file_cell) {
    FileCell nexFileCell;
    fseek(file_mem->file, virtual_to_real(file_mem, file_cell), SEEK_SET);
    fread((void *)&nexFileCell, FILE_CELL_SIZE, 1, file_mem->file);
    return nexFileCell;
}

// Creates and opens a file whose name is the string pointed to by file_name, if
// the file does not exist, otherwise returns NULL. The index has index_size
// bytes, and cells have cell_size bytes. Pre-condition: cell_size >= 4.
FileMem create_file(const char *file_name, int index_size, int cell_size) {
    FileMem file_mem = malloc(sizeof(struct _FileMem));

    // Check if the file already exists.
    file_mem->file = fopen(file_name, "r");
    if (file_mem->file != NULL) {
        // If so, abort the operation.
        fclose(file_mem->file);
        free(file_mem);
        return NULL;
    }

    file_mem->file = fopen(file_name, "w+");
    if (file_mem->file != NULL) {
        file_mem->control_info.index_size = index_size;
        file_mem->control_info.cell_size = cell_size;
        file_mem->control_info.num_cells = 0;
        file_mem->control_info.free_cells = 0;
        return file_mem;
    } else {
        free(file_mem);
        return NULL;
    }
}

// Opens the file whose name is the string pointed to by file_name, if the file
// exists, otherwise, returns NULL.
FileMem open_file(const char *file_name) {
    FileMem file_mem = (FileMem)malloc(sizeof(struct _FileMem));
    file_mem->file = fopen(file_name, "r+");
    if (file_mem->file != NULL) {
        readControlInfo(file_mem);
        return file_mem;
    } else {
        free((void *)file_mem);
        return NULL;
    }
}

// Closes the specified file.
void close_file(FileMem file_mem) {
    write_control_info(file_mem);
    fclose(file_mem->file);
    free((void *)file_mem);
}

// Reads the index from the specified file, storing it at the location given by
// index.
void read_index(FileMem file_mem, void *idx) {
    fseek(file_mem->file, CONTROL_INFO_SIZE, SEEK_SET);
    fread(idx, file_mem->control_info.index_size, 1, file_mem->file);
}

// Writes the index to the specified file, obtaining it from the location given
// by index.
void write_index(FileMem file_mem, void *idx) {
    fseek(file_mem->file, CONTROL_INFO_SIZE, SEEK_SET);
    fwrite(idx, file_mem->control_info.index_size, 1, file_mem->file);
}

// Reads from the specified file the cell whose reference is file_cell, storing
// it at the location given by cell.
void read_cell(FileMem file_mem, FileCell file_cell, void *cell) {
    if (file_cell < 0) {
        printf("Illegal FileCell: %d when reading.\n", file_cell);
        exit(1);
    }
    fseek(file_mem->file, virtual_to_real(file_mem, file_cell), SEEK_SET);
    fread(cell, file_mem->control_info.cell_size, 1, file_mem->file);
}

// Writes to the specified file the cell whose reference is file_cell, obtaining
// it from the location given by cell.
void write_cell(FileMem file_mem, FileCell file_cell, void *cell) {
    if (file_cell < 0) {
        printf("Illegal FileCell: %d when writing: %p\n", file_cell, cell);
        exit(1);
    }
    fseek(file_mem->file, virtual_to_real(file_mem, file_cell), SEEK_SET);
    fwrite(cell, file_mem->control_info.cell_size, 1, file_mem->file);
}

// Allocates memory to a new cell in the specified file, and returns a reference
// to it.
FileCell new_cell(FileMem file_mem) {
    FileCell file_cell;
    if (file_mem->control_info.free_cells == 0) {
        file_cell = ++file_mem->control_info.num_cells;
    } else {
        file_cell = file_mem->control_info.free_cells;
        file_mem->control_info.free_cells = get_next_file_cell(file_mem, file_cell);
    }
    write_control_info(file_mem); /* OPTIONAL. */
    return file_cell;
}

// Frees the memory previously allocated to the cell whose reference is
// file_cell in the specified file.
void free_cell(FileMem file_mem, FileCell file_cell) {
    fseek(file_mem->file, virtual_to_real(file_mem, file_cell), SEEK_SET);
    fwrite(&file_mem->control_info.free_cells, FILE_CELL_SIZE, 1, file_mem->file);
    file_mem->control_info.free_cells = file_cell;
    write_control_info(file_mem);
}
