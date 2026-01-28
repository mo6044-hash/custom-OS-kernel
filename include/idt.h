#ifndef IDT_H
#define IDT_H

#include "kernel.h"

#define IRQ0 32
#define IRQ1 33

typedef struct {
    u32 ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 int_no, err_code;
    u32 eip, cs, eflags, useresp, ss;
} registers_t;

void init_idt(void);
void register_interrupt_handler(u8 n, void (*handler)(void));
void isr_handler(registers_t regs);
void irq_handler(registers_t regs);
void outb(u16 port, u8 val);
u8 inb(u16 port);

#endif
