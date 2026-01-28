#include "memory.h"
#include "kernel.h"
#include "vga.h"

// Buddy allocator implementation
typedef struct buddy_block {
    struct buddy_block* next;
    u32 order;
    bool free;
} buddy_block_t;

static buddy_block_t* free_lists[BUDDY_MAX_ORDER + 1];
static u8* memory_pool;
static u32 pool_size = 1024 * 1024;  // 1MB pool
static bool initialized = false;

static u32 get_order(size_t size) {
    u32 order = 0;
    size_t block_size = BUDDY_MIN_SIZE;
    while (block_size < size && order < BUDDY_MAX_ORDER) {
        block_size <<= 1;
        order++;
    }
    return order;
}

static void* get_buddy(void* block, u32 order) {
    u32 block_size = BUDDY_MIN_SIZE << order;
    u32 offset = (u32)block - (u32)memory_pool;
    u32 buddy_offset = offset ^ block_size;
    return (void*)((u32)memory_pool + buddy_offset);
}

static void insert_block(buddy_block_t* block, u32 order) {
    block->next = free_lists[order];
    free_lists[order] = block;
    block->free = true;
    block->order = order;
}

static void remove_block(buddy_block_t* block, u32 order) {
    if (free_lists[order] == block) {
        free_lists[order] = block->next;
    } else {
        buddy_block_t* current = free_lists[order];
        while (current && current->next != block) {
            current = current->next;
        }
        if (current) {
            current->next = block->next;
        }
    }
    block->free = false;
}

static void* split_block(u32 order) {
    if (order > BUDDY_MAX_ORDER) {
        return NULL;
    }
    
    if (free_lists[order]) {
        buddy_block_t* block = free_lists[order];
        remove_block(block, order);
        return block;
    }
    
    // Try to split a larger block
    void* block = split_block(order + 1);
    if (!block) {
        return NULL;
    }
    
    buddy_block_t* buddy = (buddy_block_t*)get_buddy(block, order);
    insert_block(buddy, order);
    
    buddy_block_t* b = (buddy_block_t*)block;
    b->order = order;
    b->free = false;
    return block;
}

static void merge_block(buddy_block_t* block) {
    u32 order = block->order;
    
    while (order < BUDDY_MAX_ORDER) {
        buddy_block_t* buddy = (buddy_block_t*)get_buddy(block, order);
        
        if (buddy->free && buddy->order == order) {
            remove_block(buddy, order);
            
            // Keep the lower address block
            if (buddy < block) {
                block = buddy;
            }
            order++;
            block->order = order;
        } else {
            break;
        }
    }
    
    insert_block(block, order);
}

void memory_init(void) {
    if (initialized) return;
    
    // Allocate memory pool (simplified - in real OS, use physical memory)
    memory_pool = (u8*)0x200000;  // Start at 2MB
    pool_size = 1024 * 1024;  // 1MB
    
    // Initialize free lists
    for (int i = 0; i <= BUDDY_MAX_ORDER; i++) {
        free_lists[i] = NULL;
    }
    
    // Initialize the entire pool as one large free block
    buddy_block_t* initial_block = (buddy_block_t*)memory_pool;
    u32 max_order = BUDDY_MAX_ORDER;
    u32 pool_order = 0;
    u32 temp_size = pool_size;
    while (temp_size > BUDDY_MIN_SIZE && pool_order < BUDDY_MAX_ORDER) {
        temp_size >>= 1;
        pool_order++;
    }
    
    initial_block->order = pool_order;
    initial_block->free = true;
    initial_block->next = NULL;
    free_lists[pool_order] = initial_block;
    
    initialized = true;
}

void* kmalloc(size_t size) {
    if (!initialized) {
        memory_init();
    }
    
    if (size == 0) {
        return NULL;
    }
    
    // Add header size
    size_t total_size = size + sizeof(buddy_block_t);
    u32 order = get_order(total_size);
    
    void* block = split_block(order);
    if (!block) {
        return NULL;
    }
    
    buddy_block_t* b = (buddy_block_t*)block;
    return (void*)((u8*)block + sizeof(buddy_block_t));
}

void kfree(void* ptr) {
    if (!ptr || !initialized) {
        return;
    }
    
    buddy_block_t* block = (buddy_block_t*)((u8*)ptr - sizeof(buddy_block_t));
    merge_block(block);
}

void* krealloc(void* ptr, size_t size) {
    if (!ptr) {
        return kmalloc(size);
    }
    
    if (size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    buddy_block_t* block = (buddy_block_t*)((u8*)ptr - sizeof(buddy_block_t));
    size_t old_size = (BUDDY_MIN_SIZE << block->order) - sizeof(buddy_block_t);
    
    if (old_size >= size) {
        return ptr;
    }
    
    void* new_ptr = kmalloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, old_size < size ? old_size : size);
        kfree(ptr);
    }
    return new_ptr;
}
