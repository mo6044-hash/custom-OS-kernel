#include "idt.h"
#include "kernel.h"
#include "vga.h"

extern void (*interrupt_handlers[256])(void);

// Forward declarations for outb
void outb(u16 port, u8 val);

void isr_handler(registers_t regs) {
    if (interrupt_handlers[regs.int_no] != 0) {
        interrupt_handlers[regs.int_no]();
    } else {
        vga_puts("Unhandled interrupt: ");
        // Simple number to string conversion
        char num[32];
        int n = regs.int_no;
        int i = 0;
        if (n == 0) {
            num[i++] = '0';
        } else {
            while (n > 0) {
                num[i++] = '0' + (n % 10);
                n /= 10;
            }
        }
        num[i] = '\0';
        // Reverse
        for (int j = 0; j < i / 2; j++) {
            char tmp = num[j];
            num[j] = num[i - 1 - j];
            num[i - 1 - j] = tmp;
        }
        vga_puts(num);
        vga_puts("\n");
    }
}

void irq_handler(registers_t regs) {
    // Send EOI to PIC
    if (regs.int_no >= 40) {
        outb(0xA0, 0x20);  // Slave PIC
    }
    outb(0x20, 0x20);  // Master PIC
    
    if (interrupt_handlers[regs.int_no] != 0) {
        interrupt_handlers[regs.int_no]();
    }
}

