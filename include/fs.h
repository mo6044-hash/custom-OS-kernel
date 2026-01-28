#ifndef FS_H
#define FS_H

#include "kernel.h"

#define FS_BLOCK_SIZE 512
#define FS_MAX_FILES 64
#define FS_MAX_FILENAME 32

typedef struct {
    char name[FS_MAX_FILENAME];
    u32 size;
    u32 start_block;
    bool used;
} file_entry_t;

typedef struct {
    file_entry_t files[FS_MAX_FILES];
    u32 total_blocks;
    u32 free_blocks;
    bool initialized;
} filesystem_t;

void fs_init(void);
int fs_create_file(const char* name, u32 size);
int fs_delete_file(const char* name);
file_entry_t* fs_find_file(const char* name);
int fs_read_file(const char* name, void* buffer, u32 size);
int fs_write_file(const char* name, const void* data, u32 size);
void fs_list_files(void);

#endif
