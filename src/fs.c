#include "fs.h"
#include "kernel.h"
#include "vga.h"
#include "memory.h"

#define FS_START_ADDR 0x300000  // Start at 3MB
#define FS_SIZE (1024 * 1024)   // 1MB filesystem

static filesystem_t* fs = NULL;
static u8* fs_data = NULL;

void fs_init(void) {
    if (fs && fs->initialized) return;
    
    // Initialize filesystem structure
    fs = (filesystem_t*)kmalloc(sizeof(filesystem_t));
    if (!fs) return;
    
    fs_data = (u8*)FS_START_ADDR;
    
    // Check if filesystem exists (simple check)
    bool exists = false;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs->files[i].used) {
            exists = true;
            break;
        }
    }
    
    if (!exists) {
        // Initialize new filesystem
        memset(fs, 0, sizeof(filesystem_t));
        fs->total_blocks = FS_SIZE / FS_BLOCK_SIZE;
        fs->free_blocks = fs->total_blocks;
    }
    
    fs->initialized = true;
}

int fs_create_file(const char* name, u32 size) {
    if (!fs || !fs->initialized) return -1;
    if (strlen(name) >= FS_MAX_FILENAME) return -1;
    
    // Check if file already exists
    if (fs_find_file(name)) {
        return -1;  // File exists
    }
    
    // Find free slot
    int slot = -1;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (!fs->files[i].used) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) return -1;
    
    // Calculate blocks needed
    u32 blocks_needed = (size + FS_BLOCK_SIZE - 1) / FS_BLOCK_SIZE;
    if (blocks_needed > fs->free_blocks) {
        return -1;  // Not enough space
    }
    
    // Allocate blocks (simplified - just mark as used)
    u32 start_block = 0;
    u32 blocks_found = 0;
    for (u32 i = 0; i < fs->total_blocks && blocks_found < blocks_needed; i++) {
        bool block_used = false;
        for (int j = 0; j < FS_MAX_FILES; j++) {
            if (fs->files[j].used) {
                u32 file_start = fs->files[j].start_block;
                u32 file_blocks = (fs->files[j].size + FS_BLOCK_SIZE - 1) / FS_BLOCK_SIZE;
                if (i >= file_start && i < file_start + file_blocks) {
                    block_used = true;
                    break;
                }
            }
        }
        if (!block_used) {
            if (blocks_found == 0) {
                start_block = i;
            }
            blocks_found++;
        }
    }
    
    if (blocks_found < blocks_needed) {
        return -1;  // Couldn't find contiguous blocks
    }
    
    // Create file entry
    strcpy(fs->files[slot].name, name);
    fs->files[slot].size = size;
    fs->files[slot].start_block = start_block;
    fs->files[slot].used = true;
    
    fs->free_blocks -= blocks_needed;
    
    return 0;
}

int fs_delete_file(const char* name) {
    if (!fs || !fs->initialized) return -1;
    
    file_entry_t* file = fs_find_file(name);
    if (!file) return -1;
    
    u32 blocks_used = (file->size + FS_BLOCK_SIZE - 1) / FS_BLOCK_SIZE;
    fs->free_blocks += blocks_used;
    
    memset(file, 0, sizeof(file_entry_t));
    
    return 0;
}

file_entry_t* fs_find_file(const char* name) {
    if (!fs || !fs->initialized) return NULL;
    
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs->files[i].used && strcmp(fs->files[i].name, name) == 0) {
            return &fs->files[i];
        }
    }
    return NULL;
}

int fs_read_file(const char* name, void* buffer, u32 size) {
    if (!fs || !fs->initialized) return -1;
    
    file_entry_t* file = fs_find_file(name);
    if (!file) return -1;
    
    u32 read_size = size < file->size ? size : file->size;
    u32 offset = file->start_block * FS_BLOCK_SIZE;
    
    memcpy(buffer, fs_data + offset, read_size);
    
    return read_size;
}

int fs_write_file(const char* name, const void* data, u32 size) {
    if (!fs || !fs->initialized) return -1;
    
    file_entry_t* file = fs_find_file(name);
    if (!file) return -1;
    
    if (size > file->size) {
        size = file->size;  // Don't write beyond file size
    }
    
    u32 offset = file->start_block * FS_BLOCK_SIZE;
    memcpy(fs_data + offset, data, size);
    
    return size;
}

void fs_list_files(void) {
    if (!fs || !fs->initialized) {
        vga_puts("Filesystem not initialized\n");
        return;
    }
    
    bool found = false;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs->files[i].used) {
            vga_puts(fs->files[i].name);
            vga_puts(" (");
            // Print size (simplified)
            char size_str[32];
            u32 size = fs->files[i].size;
            int idx = 0;
            if (size == 0) {
                size_str[idx++] = '0';
            } else {
                u32 temp = size;
                while (temp > 0) {
                    size_str[idx++] = '0' + (temp % 10);
                    temp /= 10;
                }
                // Reverse
                for (int j = 0; j < idx / 2; j++) {
                    char tmp = size_str[j];
                    size_str[j] = size_str[idx - 1 - j];
                    size_str[idx - 1 - j] = tmp;
                }
            }
            size_str[idx] = '\0';
            vga_puts(size_str);
            vga_puts(" bytes)\n");
            found = true;
        }
    }
    
    if (!found) {
        vga_puts("No files found\n");
    }
}
