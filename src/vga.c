#include "vga.h"
#include "kernel.h"

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

static u8 terminal_row;
static u8 terminal_column;
static u8 terminal_color;
static u16* terminal_buffer;

static inline u8 vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline u16 vga_entry(unsigned char uc, u8 color) {
    return (u16)uc | (u16)color << 8;
}

void vga_init(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (u16*)VGA_MEMORY;
    vga_clear();
}

void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
}

void vga_set_color(u8 color) {
    terminal_color = color;
}

u8 vga_get_color(void) {
    return terminal_color;
}

void vga_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            // Scroll
            for (size_t y = 1; y < VGA_HEIGHT; y++) {
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                    terminal_buffer[(y - 1) * VGA_WIDTH + x] = 
                        terminal_buffer[y * VGA_WIDTH + x];
                }
            }
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = 
                    vga_entry(' ', terminal_color);
            }
            terminal_row = VGA_HEIGHT - 1;
        }
    } else {
        const size_t index = terminal_row * VGA_WIDTH + terminal_column;
        terminal_buffer[index] = vga_entry(c, terminal_color);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                // Scroll
                for (size_t y = 1; y < VGA_HEIGHT; y++) {
                    for (size_t x = 0; x < VGA_WIDTH; x++) {
                        terminal_buffer[(y - 1) * VGA_WIDTH + x] = 
                            terminal_buffer[y * VGA_WIDTH + x];
                    }
                }
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                    terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = 
                        vga_entry(' ', terminal_color);
                }
                terminal_row = VGA_HEIGHT - 1;
            }
        }
    }
}

void vga_puts(const char* str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        vga_putchar(str[i]);
    }
}
