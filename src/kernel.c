#include "kernel.h"
#include "gdt.h"
#include "idt.h"
#include "memory.h"
#include "scheduler.h"
#include "keyboard.h"
#include "vga.h"
#include "fs.h"
#include "shell.h"

void kernel_main(void) {
    // Initialize VGA
    vga_init();
    vga_clear();
    vga_puts("Custom OS Kernel v1.0\n");
    vga_puts("Initializing system...\n");
    
    // Initialize memory management
    vga_puts("Initializing memory manager...\n");
    memory_init();
    
    // Initialize file system
    vga_puts("Initializing file system...\n");
    fs_init();
    
    // Initialize keyboard
    vga_puts("Initializing keyboard driver...\n");
    keyboard_init();
    
    // Initialize scheduler
    vga_puts("Initializing scheduler...\n");
    scheduler_init();
    
    vga_puts("\nSystem initialized successfully!\n");
    vga_puts("Starting shell...\n\n");
    
    // Start shell
    shell_run();
    
    // Should never reach here
    while(1) {
        asm volatile("hlt");
    }
}
