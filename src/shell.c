#include "shell.h"
#include "kernel.h"
#include "vga.h"
#include "keyboard.h"
#include "fs.h"
#include "scheduler.h"

static void cmd_help(void) {
    vga_puts("Available commands:\n");
    vga_puts("  help     - Show this help message\n");
    vga_puts("  clear    - Clear the screen\n");
    vga_puts("  ls       - List files\n");
    vga_puts("  create   - Create a file\n");
    vga_puts("  delete   - Delete a file\n");
    vga_puts("  echo     - Echo text\n");
    vga_puts("  exit     - Exit shell (not implemented)\n");
}

static void cmd_clear(void) {
    vga_clear();
}

static void cmd_ls(void) {
    fs_list_files();
}

static void cmd_create(char* args) {
    if (!args || strlen(args) == 0) {
        vga_puts("Usage: create <filename> <size>\n");
        return;
    }
    
    char filename[FS_MAX_FILENAME];
    u32 size = 0;
    
    // Simple parsing
    int i = 0;
    int j = 0;
    while (args[i] == ' ') i++;
    while (args[i] != ' ' && args[i] != '\0' && j < FS_MAX_FILENAME - 1) {
        filename[j++] = args[i++];
    }
    filename[j] = '\0';
    
    while (args[i] == ' ') i++;
    while (args[i] >= '0' && args[i] <= '9') {
        size = size * 10 + (args[i] - '0');
        i++;
    }
    
    if (size == 0) {
        size = 1024;  // Default size
    }
    
    if (fs_create_file(filename, size) == 0) {
        vga_puts("File created: ");
        vga_puts(filename);
        vga_puts("\n");
    } else {
        vga_puts("Failed to create file\n");
    }
}

static void cmd_delete(char* args) {
    if (!args || strlen(args) == 0) {
        vga_puts("Usage: delete <filename>\n");
        return;
    }
    
    char filename[FS_MAX_FILENAME];
    int i = 0;
    int j = 0;
    while (args[i] == ' ') i++;
    while (args[i] != ' ' && args[i] != '\0' && j < FS_MAX_FILENAME - 1) {
        filename[j++] = args[i++];
    }
    filename[j] = '\0';
    
    if (fs_delete_file(filename) == 0) {
        vga_puts("File deleted: ");
        vga_puts(filename);
        vga_puts("\n");
    } else {
        vga_puts("File not found\n");
    }
}

static void cmd_echo(char* args) {
    if (args) {
        vga_puts(args);
    }
    vga_puts("\n");
}

void shell_execute(const char* input) {
    if (!input || strlen(input) == 0) {
        return;
    }
    
    char cmd[SHELL_MAX_INPUT];
    char args[SHELL_MAX_INPUT];
    
    // Parse command and arguments
    int i = 0;
    while (input[i] == ' ') i++;  // Skip leading spaces
    
    int cmd_len = 0;
    while (input[i] != ' ' && input[i] != '\0' && cmd_len < SHELL_MAX_INPUT - 1) {
        cmd[cmd_len++] = input[i++];
    }
    cmd[cmd_len] = '\0';
    
    int args_len = 0;
    while (input[i] == ' ') i++;  // Skip spaces
    while (input[i] != '\0' && args_len < SHELL_MAX_INPUT - 1) {
        args[args_len++] = input[i++];
    }
    args[args_len] = '\0';
    
    // Execute command
    if (strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (strcmp(cmd, "clear") == 0) {
        cmd_clear();
    } else if (strcmp(cmd, "ls") == 0) {
        cmd_ls();
    } else if (strcmp(cmd, "create") == 0) {
        cmd_create(args);
    } else if (strcmp(cmd, "delete") == 0) {
        cmd_delete(args);
    } else if (strcmp(cmd, "echo") == 0) {
        cmd_echo(args);
    } else if (strcmp(cmd, "exit") == 0) {
        vga_puts("Exit not implemented\n");
    } else if (cmd_len > 0) {
        vga_puts("Unknown command: ");
        vga_puts(cmd);
        vga_puts("\nType 'help' for available commands\n");
    }
}

void shell_run(void) {
    char input[SHELL_MAX_INPUT];
    int input_pos = 0;
    
    vga_puts("Custom OS Shell v1.0\n");
    vga_puts("Type 'help' for available commands\n");
    
    while (1) {
        vga_puts("> ");
        
        input_pos = 0;
        memset(input, 0, SHELL_MAX_INPUT);
        
        // Read input
        while (input_pos < SHELL_MAX_INPUT - 1) {
            char c = keyboard_getchar();
            
            if (c == '\n') {
                vga_putchar('\n');
                break;
            } else if (c == '\b') {
                if (input_pos > 0) {
                    input_pos--;
                    input[input_pos] = '\0';
                    vga_putchar('\b');
                    vga_putchar(' ');
                    vga_putchar('\b');
                }
            } else if (c >= 32 && c < 127) {
                input[input_pos++] = c;
                vga_putchar(c);
            }
        }
        
        shell_execute(input);
    }
}
