# Testing Guide for Custom OS Kernel

## Prerequisites

You need the following tools installed:

1. **GCC** with 32-bit support
2. **NASM** (Netwide Assembler)
3. **QEMU** (for running the OS)
4. **GRUB tools** (for creating bootable ISO)

## Installation (macOS)

### Using Homebrew

```bash
# Install Homebrew if you don't have it
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install required tools
brew install nasm qemu

# For 32-bit GCC support, you may need:
brew install gcc@11  # or another version with 32-bit support
```

**Note:** On newer macOS systems, you might need to install a cross-compiler for 32-bit support. If `gcc -m32` doesn't work, you can:

1. Use a Docker container with a Linux environment
2. Use a Linux VM
3. Install a cross-compiler toolchain

### Alternative: Using Docker (Recommended for macOS)

If you have Docker installed, you can use a Linux container:

```bash
# Create a Dockerfile (see below)
docker build -t os-kernel-build .
docker run -v $(pwd):/workspace -it os-kernel-build
```

## Building the Kernel

Once dependencies are installed:

```bash
cd /Users/Meshack/Desktop/projects/systems/custom-os-kernel
make clean  # Clean any previous builds
make        # Build the kernel
```

**Expected output:**
- Object files created in `build/` directory
- `build/kernel.bin` - The compiled kernel
- `iso/os.iso` - Bootable ISO image

## Running the Kernel

### Option 1: Using QEMU (Recommended)

```bash
make run
# or
qemu-system-i386 -cdrom iso/os.iso -serial stdio
```

### Option 2: Using QEMU with GUI

```bash
qemu-system-i386 -cdrom iso/os.iso
```

### Option 3: Using VirtualBox

1. Create a new VM (Type: Other, Version: Other/Unknown)
2. Set memory to at least 64MB
3. Add the ISO (`iso/os.iso`) as a bootable disk
4. Start the VM

## What You Should See

If everything is working correctly, you should see:

1. **Boot Screen**: GRUB menu (may flash quickly)
2. **Kernel Initialization**:
   ```
   Custom OS Kernel v1.0
   Initializing system...
   Initializing memory manager...
   Initializing file system...
   Initializing keyboard driver...
   Initializing scheduler...
   
   System initialized successfully!
   Starting shell...
   
   > 
   ```

3. **Interactive Shell**: You can type commands:
   - `help` - Show available commands
   - `clear` - Clear the screen
   - `ls` - List files
   - `create test.txt 1024` - Create a file
   - `echo Hello World` - Echo text
   - `delete test.txt` - Delete a file

## Troubleshooting

### Build Errors

**Error: "gcc: error: unrecognized command line option '-m32'"**
- Solution: Install a 32-bit cross-compiler or use Docker/Linux

**Error: "nasm: command not found"**
- Solution: `brew install nasm`

**Error: "qemu-system-i386: command not found"**
- Solution: `brew install qemu`

### Runtime Issues

**Kernel doesn't boot:**
- Check that `iso/os.iso` was created successfully
- Verify GRUB configuration
- Try rebuilding: `make clean && make`

**No output on screen:**
- Try: `qemu-system-i386 -cdrom iso/os.iso -serial stdio -monitor stdio`
- Check QEMU version compatibility

**Keyboard not working:**
- Make sure you're typing in the QEMU window
- Try: `qemu-system-i386 -cdrom iso/os.iso -enable-kvm` (Linux only)

## Quick Test Script

Create a file `test.sh`:

```bash
#!/bin/bash
echo "Building kernel..."
make clean
make

if [ $? -eq 0 ]; then
    echo "Build successful! Starting QEMU..."
    make run
else
    echo "Build failed!"
    exit 1
fi
```

Make it executable:
```bash
chmod +x test.sh
./test.sh
```

## Expected Behavior

✅ **Working correctly if:**
- Kernel boots without errors
- You see initialization messages
- Shell prompt appears (`>`)
- Commands execute correctly
- Keyboard input works
- File operations work

❌ **Not working if:**
- QEMU shows "Boot failed"
- Screen is blank/black
- Kernel panics or triple-faults
- No response to keyboard input

## Performance Notes

- First boot may take a moment
- Context switching is optimized for minimal overhead
- Memory allocation is efficient with buddy algorithm
- File system operations are fast for small files

## Next Steps

Once you confirm it's working:
1. Try creating multiple files
2. Test different shell commands
3. Experiment with the scheduler (if you add process creation)
4. Modify and rebuild to see changes
