# Custom OS Kernel - Project Summary

## Overview
This is a complete custom operating system kernel implementation built from scratch in C and x86 Assembly. The kernel features a monolithic architecture with all core OS components integrated.

## Components Implemented

### 1. Boot System
- **boot.asm**: Multiboot-compliant boot code
- **GRUB Integration**: Bootable via GRUB bootloader
- **Kernel Entry**: Proper initialization sequence

### 2. Memory Management
- **Buddy Allocator**: Dynamic memory allocation algorithm
  - Minimum block size: 16 bytes
  - Maximum block size: 1024 bytes
  - 10 allocation orders
  - Efficient block splitting and merging

### 3. Process Scheduling
- **Round-Robin with Priority Queues**:
  - 4 priority levels (0-3, where 3 is highest)
  - Separate ready queue for each priority
  - Context switching support
  - Process state management (READY, RUNNING, BLOCKED, TERMINATED)

### 4. Interrupt Handling
- **32 Exception Handlers** (ISR 0-31)
- **Hardware Interrupt Handlers** (IRQ 0-1)
- **PIC Remapping**: Programmable Interrupt Controller configuration
- **Interrupt-driven I/O**: Keyboard input via interrupts

### 5. Device Drivers
- **PS/2 Keyboard Driver**:
  - Scancode to ASCII conversion
  - Interrupt-driven input
  - Input buffer management
  
- **VGA Text Mode Driver**:
  - 80x25 text mode display
  - Character output with scrolling
  - Color support

### 6. File System
- **Simple FAT-like File System**:
  - Block-based storage (512 bytes per block)
  - File metadata (name, size, location)
  - Basic operations: create, delete, read, write, list
  - Maximum 64 files supported

### 7. Shell
- **Interactive Command Shell**:
  - Command parsing and execution
  - Built-in commands: help, clear, ls, create, delete, echo
  - Keyboard input handling
  - Text output to VGA display

## Build System
- **Makefile**: Automated build process
- **Linker Script**: Memory layout configuration
- **ISO Generation**: Bootable disk image creation

## File Structure
```
custom-os-kernel/
├── src/              # Source files
│   ├── boot.asm     # Boot code
│   ├── kernel.c     # Kernel entry
│   ├── gdt.c        # Global Descriptor Table
│   ├── idt.c        # Interrupt Descriptor Table
│   ├── isr.asm      # Interrupt Service Routines
│   ├── isr_handler.c # ISR handlers
│   ├── memory.c     # Buddy allocator
│   ├── scheduler.c  # Process scheduler
│   ├── keyboard.c   # Keyboard driver
│   ├── vga.c        # VGA driver
│   ├── fs.c         # File system
│   ├── shell.c      # Shell
│   └── util.c       # Utilities
├── include/         # Header files
├── Makefile         # Build system
├── linker.ld        # Linker script
├── grub.cfg         # GRUB config
└── README.md        # Documentation
```

## Technical Highlights

### Performance Optimizations
- Priority-based scheduling reduces context switch overhead
- Buddy allocator minimizes memory fragmentation
- Efficient interrupt handling with minimal latency

### Architecture Decisions
- Monolithic kernel for simplicity and performance
- Round-robin scheduling with priorities for fairness
- Buddy allocator for predictable memory allocation
- Simple file system suitable for embedded systems

## Usage

### Building
```bash
make
```

### Running
```bash
make run
# or
qemu-system-i386 -cdrom iso/os.iso -serial stdio
```

### Shell Commands
- `help` - Show available commands
- `clear` - Clear screen
- `ls` - List files
- `create <filename> <size>` - Create file
- `delete <filename>` - Delete file
- `echo <text>` - Echo text

## Future Enhancements
- Virtual memory and paging
- Multi-level file system with directories
- System calls interface
- Networking stack
- Advanced process management (fork, exec, wait)
- Real-time scheduling algorithms

## Notes
This kernel is designed for educational purposes and demonstrates core OS concepts. It runs in a controlled environment (QEMU) and is not intended for production use.
