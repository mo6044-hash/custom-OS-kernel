#ifndef MEMORY_H
#define MEMORY_H

#include "kernel.h"

#define BUDDY_MAX_ORDER 10
#define BUDDY_MIN_SIZE (1 << 4)  // 16 bytes minimum
#define BUDDY_MAX_SIZE (1 << BUDDY_MAX_ORDER)  // 1024 bytes maximum

void memory_init(void);
void* kmalloc(size_t size);
void kfree(void* ptr);
void* krealloc(void* ptr, size_t size);

#endif
