# Custom Operating System Kernel

A monolithic kernel implemented from scratch in C and x86 Assembly, featuring round-robin process scheduling with priority queues, buddy algorithm for dynamic memory allocation, and a simple FAT-like file system.

## Features

- **Monolithic Kernel Architecture**: Complete kernel implementation in C and x86 Assembly
- **Process Scheduling**: Round-robin scheduler with priority queues (4 priority levels)
- **Memory Management**: Buddy algorithm for efficient dynamic memory allocation
- **File System**: Simple FAT-like file system for persistent storage
- **Interrupt Handling**: x86 interrupt service routines (ISRs) for hardware interrupts
- **Device Drivers**:
  - PS/2 keyboard input driver
  - VGA text mode display driver
- **Boot System**: Bootable via GRUB bootloader
- **Shell**: Interactive shell with basic commands

## Project Structure

```
custom-os-kernel/
├── src/              # Source files
│   ├── boot.asm      # Boot code with Multiboot header
│   ├── kernel.c      # Kernel entry point
│   ├── gdt.c         # Global Descriptor Table
│   ├── idt.c         # Interrupt Descriptor Table
│   ├── isr.asm       # Interrupt Service Routines
│   ├── isr_handler.c # ISR handlers
│   ├── memory.c      # Buddy allocator
│   ├── scheduler.c   # Process scheduler
│   ├── keyboard.c    # PS/2 keyboard driver
│   ├── vga.c         # VGA text mode driver
│   ├── fs.c          # File system
│   ├── shell.c       # Shell implementation
│   └── util.c        # Utility functions
├── include/          # Header files
├── build/            # Build output (generated)
├── iso/              # ISO image (generated)
├── Makefile          # Build system
├── linker.ld         # Linker script
└── grub.cfg          # GRUB configuration
```

## Requirements

- **GCC** (with 32-bit support): `gcc -m32`
- **NASM**: Netwide Assembler
- **LD**: GNU Linker
- **GRUB**: GRUB bootloader tools (`grub-mkrescue` or `grub2-mkrescue`)
- **QEMU**: For running the OS (`qemu-system-i386`)

### Installing Dependencies

#### macOS
```bash
brew install gcc nasm qemu
# GRUB may need to be installed separately or use genisoimage
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install build-essential nasm qemu-system-x86 grub-pc-bin xorriso
```

#### Linux (Fedora)
```bash
sudo dnf install gcc nasm qemu-system-x86 grub2-tools xorriso
```

## Building

1. Clone the repository:
```bash
git clone https://github.com/mo6044-hash/custom-OS-kernel.git
cd custom-OS-kernel
```

2. Build the kernel:
```bash
make
```

This will:
- Compile all C and Assembly source files
- Link them into `build/kernel.bin`
- Create a bootable ISO image at `iso/os.iso`

3. Clean build artifacts:
```bash
make clean
```

## Running

### Using QEMU (Recommended)

```bash
make run
# or
qemu-system-i386 -cdrom iso/os.iso -serial stdio
```

### Using VirtualBox or VMware

1. Create a new virtual machine
2. Set the ISO (`iso/os.iso`) as the boot medium
3. Start the VM

## Shell Commands

Once the kernel boots, you'll see an interactive shell. Available commands:

- `help` - Show available commands
- `clear` - Clear the screen
- `ls` - List files in the file system
- `create <filename> <size>` - Create a new file
- `delete <filename>` - Delete a file
- `echo <text>` - Echo text to the screen

## Technical Details

### Memory Management

The kernel uses a **buddy allocator** for dynamic memory allocation:
- Minimum block size: 16 bytes
- Maximum block size: 1024 bytes
- 10 allocation orders
- Efficient splitting and merging of blocks

### Process Scheduling

The scheduler implements **round-robin with priority queues**:
- 4 priority levels (0-3, where 3 is highest)
- Each priority level has its own ready queue
- Higher priority processes are scheduled first
- Context switching optimized for minimal overhead

### File System

Simple FAT-like file system:
- Block size: 512 bytes
- Maximum files: 64
- Maximum filename length: 32 characters
- Basic file operations: create, delete, read, write, list

### Interrupt Handling

- 32 exception handlers (ISR 0-31)
- Hardware interrupt handlers (IRQ 0-1)
- Programmable Interrupt Controller (PIC) remapping
- Interrupt-driven keyboard input

## Performance Optimizations

The kernel includes several optimizations:
- Priority-based scheduling reduces context switch overhead
- Buddy allocator minimizes fragmentation
- Efficient interrupt handling with minimal latency

## Limitations

This is an educational kernel implementation with some limitations:
- No virtual memory/paging
- Limited file system (no directories)
- Basic process management (no fork/exec)
- No networking support
- Simplified memory management (no physical memory mapping)

## Future Enhancements

Potential improvements:
- Virtual memory and paging
- Multi-level file system with directories
- System calls interface
- Networking stack
- Advanced process management (fork, exec, wait)
- Device driver framework
- Real-time scheduling algorithms

## License

This project is for educational purposes. Feel free to use and modify as needed.

## Author

GitHub: [mo6044-hash](https://github.com/mo6044-hash)

## Acknowledgments

This kernel implementation is inspired by various OS development tutorials and resources, including:
- OSDev.org wiki
- JamesM's kernel development tutorials
- Various open-source kernel projects
