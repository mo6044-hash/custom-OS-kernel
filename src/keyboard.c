#include "keyboard.h"
#include "idt.h"
#include "kernel.h"
#include "vga.h"

#define KEYBOARD_BUFFER_SIZE 256

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static u32 keyboard_head = 0;
static u32 keyboard_tail = 0;
static bool keyboard_initialized = false;

// PS/2 keyboard scancode to ASCII mapping (US layout)
static const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4',
    '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void keyboard_handler(void) {
    u8 scancode = inb(KEYBOARD_DATA_PORT);
    
    if (scancode & 0x80) {
        // Key release - ignore for now
        return;
    }
    
    char c = scancode_to_ascii[scancode];
    if (c != 0) {
        u32 next = (keyboard_tail + 1) % KEYBOARD_BUFFER_SIZE;
        if (next != keyboard_head) {
            keyboard_buffer[keyboard_tail] = c;
            keyboard_tail = next;
        }
    }
}

void keyboard_init(void) {
    if (keyboard_initialized) return;
    
    keyboard_head = 0;
    keyboard_tail = 0;
    memset(keyboard_buffer, 0, KEYBOARD_BUFFER_SIZE);
    
    // Register keyboard interrupt handler
    register_interrupt_handler(IRQ1, keyboard_handler);
    
    keyboard_initialized = true;
}

char keyboard_getchar(void) {
    while (!keyboard_has_input()) {
        asm volatile("hlt");
    }
    
    char c = keyboard_buffer[keyboard_head];
    keyboard_head = (keyboard_head + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

bool keyboard_has_input(void) {
    return keyboard_head != keyboard_tail;
}
