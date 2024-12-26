#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


#define FIFO_DIV "fifo_div"
#define FIFO_RESULT "fifo_result"

int main() {
    int num1, num2, result;
    int my_fifo;

    while (1) { 
        // Open FIFO for reading numbers we got from the calculator.c file
        my_fifo = open(FIFO_DIV, O_RDONLY);
        if (my_fifo < 0) {
            perror("Failed to open FIFO_ADD for reading");
            exit(EXIT_FAILURE);
        }

        // Read numbers
        if (read(my_fifo, &num1, sizeof(num1)) <= 0 || read(my_fifo, &num2, sizeof(num2)) <= 0) {
            // If the read fails,I close and continue waiting
            close(my_fifo);
            continue;
        }
        close(my_fifo);

        
        
  
    if (num2 != 0) {
        result = num1 / num2;
    } else {
        result = 0;  // Error handling for division by zero control to avoid error.
    }

 

        // Open FIFO for writing the result
        my_fifo = open(FIFO_RESULT, O_WRONLY);
        if (my_fifo < 0) {
            perror("Failed to open FIFO_RESULT for writing");
            exit(EXIT_FAILURE);
        }

        if (write(my_fifo, &result, sizeof(result)) <= 0) {
            perror("Failed to write result to FIFO_RESULT");
        }
        close(my_fifo);


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

