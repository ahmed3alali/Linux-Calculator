#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_MUL "fifo_mul"
#define FIFO_RESULT "fifo_result"

int main() {
    int num1, num2, result;
    int mult_fd;

    while (1) { 
        // Open FIFO for reading numbers
        mult_fd = open(FIFO_MUL, O_RDONLY);
        if (mult_fd < 0) {
            perror("Failed to open FIFO_ADD for reading");
            exit(EXIT_FAILURE);
        }

        // Read numbers
        if (read(mult_fd, &num1, sizeof(num1)) <= 0 || read(mult_fd, &num2, sizeof(num2)) <= 0) {
            // If the read fails, close and continue waiting
            close(mult_fd);
            continue;
        }
        close(mult_fd);

        result = num1 * num2;

        // Open FIFO for writing the result
        mult_fd = open(FIFO_RESULT, O_WRONLY);
        if (mult_fd < 0) {
            perror("Failed to open FIFO_RESULT for writing");
            exit(EXIT_FAILURE);
        }

        if (write(mult_fd, &result, sizeof(result)) <= 0) {
            perror("Failed to write result to FIFO_RESULT");
        }
        close(mult_fd);
        
        char val_str[100];
        snprintf(val_str, sizeof(val_str), "%d", result);


        
        pid_t pid = fork();
        if (pid == 0) {
            execl("./saver", "saver",val_str, NULL);
            perror("Failed to exec saver");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

