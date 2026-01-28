# Makefile for Custom OS Kernel

# Toolchain
AS = nasm
CC = gcc
LD = ld

# Flags
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -Wall -Wextra -std=c99 -I./include
LDFLAGS = -m elf_i386 -T linker.ld

# Directories
SRC_DIR = src
BUILD_DIR = build
ISO_DIR = iso
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub

# Source files
ASM_SOURCES = $(shell find $(SRC_DIR) -name "*.asm" 2>/dev/null)
C_SOURCES = $(shell find $(SRC_DIR) -name "*.c" 2>/dev/null)

# Object files
ASM_OBJECTS = $(ASM_SOURCES:$(SRC_DIR)/%.asm=$(BUILD_DIR)/%.o)
C_OBJECTS = $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

# Output
KERNEL = $(BUILD_DIR)/kernel.bin
ISO = $(ISO_DIR)/os.iso

.PHONY: all clean run qemu iso

all: $(ISO)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

$(ISO): $(KERNEL) grub.cfg
	@mkdir -p $(GRUB_DIR)
	cp $(KERNEL) $(BOOT_DIR)/
	cp grub.cfg $(GRUB_DIR)/
	grub-mkrescue -o $(ISO) $(ISO_DIR) 2>/dev/null || \
	grub2-mkrescue -o $(ISO) $(ISO_DIR) 2>/dev/null || \
	genisoimage -R -b boot/grub/eltorito.img -no-emul-boot -boot-load-size 4 \
		-A os -input-charset utf8 -quiet -boot-info-table -o $(ISO) $(ISO_DIR)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio

qemu: run

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)
