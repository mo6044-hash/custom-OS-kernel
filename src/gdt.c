#include "gdt.h"
#include "kernel.h"

struct gdt_entry {
    u16 limit_low;
    u16 base_low;
    u8  base_middle;
    u8  access;
    u8  granularity;
    u8  base_high;
} __attribute__((packed));

struct gdt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

struct gdt_entry gdt[5];
struct gdt_ptr gdtp;

extern void gdt_flush(u32);

static void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    
    gdt[num].access = access;
}

void init_gdt(void) {
    gdtp.limit = (sizeof(struct gdt_entry) * 5) - 1;
    gdtp.base = (u32)&gdt;
    
    // Null segment
    gdt_set_gate(0, 0, 0, 0, 0);
    
    // Code segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    // Data segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    
    // User code segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    
    // User data segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    
    gdt_flush((u32)&gdtp);
}
