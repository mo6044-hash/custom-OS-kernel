; Boot code with Multiboot header
[BITS 32]
[ORG 0x100000]

MULTIBOOT_PAGE_ALIGN   equ 1<<0
MULTIBOOT_MEMORY_INFO  equ 1<<1
MULTIBOOT_VIDEO_MODE   equ 1<<2
MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
CHECKSUM               equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

section .multiboot
align 4
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd CHECKSUM

section .text
global boot
extern kernel_main
extern init_gdt
extern init_idt

boot:
    ; Set up stack
    mov esp, stack_top
    
    ; Save multiboot info
    push ebx
    push eax
    
    ; Initialize GDT
    call init_gdt
    
    ; Initialize IDT
    call init_idt
    
    ; Enable interrupts
    sti
    
    ; Call kernel main
    call kernel_main
    
    ; Halt if kernel returns
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384 ; 16KB stack
stack_top:
