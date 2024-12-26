#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_ADD "fifo_add"
#define FIFO_RESULT "fifo_result"

int main() {
    int num1, num2, result;
    int add_fd;

    while (1) { 
        // Open FIFO for reading numbers
        add_fd = open(FIFO_ADD, O_RDONLY);
        if (add_fd < 0) {
            perror("Failed to open FIFO_ADD for reading");
            exit(EXIT_FAILURE);
        }

        // Read numbers
        if (read(add_fd, &num1, sizeof(num1)) <= 0 || read(add_fd, &num2, sizeof(num2)) <= 0) {
            // If the read fails, close and continue waiting
            close(add_fd);
            continue;
        }
        close(add_fd);

        result = num1 + num2;

        // Open FIFO for writing the result
        add_fd = open(FIFO_RESULT, O_WRONLY);
        if (add_fd < 0) {
            perror("Failed to open FIFO_RESULT for writing");
            exit(EXIT_FAILURE);
        }

        if (write(add_fd, &result, sizeof(result)) <= 0) {
            perror("Failed writing result to FIFO_RESULT");
        }
        close(add_fd);
char val_str[100];
        snprintf(val_str, sizeof(val_str), "%d", result);

        // Call saver program
        pid_t pid = fork();
        if (pid == 0) {
            execl("./saver", "saver",val_str, NULL);
            perror("Failed to exec saver");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

