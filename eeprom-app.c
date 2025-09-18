#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <errno.h>

// The path to the sysfs file created by the at24 driver
#define EEPROM_FILE_PATH "/sys/bus/i2c/devices/2-0050/eeprom"
#define MAX_DATA_SIZE 256

int main() {
    int fd;
    unsigned int address;
    char write_buffer[MAX_DATA_SIZE];
    char read_buffer[MAX_DATA_SIZE];
    ssize_t bytes_read, bytes_written;

    // Open the EEPROM file for reading and writing once at the beginning
    fd = open(EEPROM_FILE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the EEPROM file. Please run with sudo.");
        return EXIT_FAILURE;
    }

    printf("--- Interactive EEPROM I/O ---\n");
    printf("Type 'q' or 'quit' to exit.\n\n");

    while (1) {
        // --- 1. Get user input for address ---
        printf("Enter a memory address in hex (e.g., 0x10): ");
        if (scanf("%x", &address) != 1) {
            // Check for exit condition
            char exit_cmd[10];
            scanf("%s", exit_cmd);
            if (strcmp(exit_cmd, "q") == 0 || strcmp(exit_cmd, "quit") == 0) {
                break;
            }
            printf("Invalid input. Please enter a hex address.\n");
            // Clear the input buffer
            while (getchar() != '\n');
            continue;
        };

        // --- 5. Move the file pointer back to the same address for reading ---
        if (lseek(fd, address, SEEK_SET) == -1) {
            perror("Failed to seek to the target address for reading");
            continue;
        }

        // --- 6. Read data back from the EEPROM ---
        bytes_read = read(fd, read_buffer, sizeof(read_buffer) - 1);
        if (bytes_read < 0) {
            perror("Failed to read from the EEPROM");
            continue;
        }
        
        // Add a null terminator
        read_buffer[bytes_read] = '\0';
        printf("Read %ld bytes from the EEPROM. Data: '%s'\n\n", bytes_read, read_buffer);
    }

    // Close the file when the loop is exited
    close(fd);
    printf("Program finished. File closed.\n");

    return EXIT_SUCCESS;
}
