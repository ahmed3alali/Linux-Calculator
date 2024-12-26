#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define FIFO_ADD "fifo_add"
#define FIFO_SUB "fifo_sub"
#define FIFO_MUL "fifo_mul"
#define FIFO_DIV "fifo_div"
#define FIFO_RESULT "fifo_result"

void show_menu() {
    printf("Calculator Menu:\n");
    printf("1. Addition\n");
    printf("2. Subtraction\n");
    printf("3. Multiplication\n");
    printf("4. Division\n");
    printf("5. Exit\n");
    printf("Choose an option: ");
}

int main() {
    int choice;
    pid_t pid_add, pid_sub, pid_mul, pid_div;
    int num1, num2;

    // Create FIFOs
    mkfifo(FIFO_ADD, 0666);
    mkfifo(FIFO_SUB, 0666);
    mkfifo(FIFO_MUL, 0666);
    mkfifo(FIFO_DIV, 0666);
    mkfifo(FIFO_RESULT, 0666); // FIFO for reading results

    // Forking 
    pid_add = fork();
    if (pid_add == 0) {
        execl("./addition", "addition", NULL);
    }

    pid_sub = fork();
    if (pid_sub == 0) {
        execl("./subtraction", "subtraction", NULL);
    }

    pid_mul = fork();
    if (pid_mul == 0) {
        execl("./multiplication", "multiplication", NULL);
    }

    pid_div = fork();
    if (pid_div == 0) {
        execl("./division", "division", NULL);
    }

    // Parent process: show menu and take input
    while (1) {
        show_menu();
        scanf("%d", &choice);

        if (choice == 5) {
            break;
        }

        printf("Enter first number: ");
        scanf("%d", &num1);
        printf("Enter second number: ");
        scanf("%d", &num2);

        // Sending numbers to corresponding FIFO
        int fifo_fd;
        switch (choice) {
            case 1:
                fifo_fd = open(FIFO_ADD, O_WRONLY);
                break;
            case 2:
                fifo_fd = open(FIFO_SUB, O_WRONLY);
                break;
            case 3:
                fifo_fd = open(FIFO_MUL, O_WRONLY);
                break;
            case 4:
                fifo_fd = open(FIFO_DIV, O_WRONLY);
                break;
            default:
                printf("Invalid choice.\n");
                continue;
        }

        if (fifo_fd < 0) {
            perror("Failed to open operation FIFO");
            continue;
        }

        write(fifo_fd, &num1, sizeof(num1));
        write(fifo_fd, &num2, sizeof(num2));
        close(fifo_fd);

        // Open FIFO_RESULT for reading the result
        int result_fd = open(FIFO_RESULT, O_RDONLY);
        if (result_fd < 0) {
            perror("Failed to open FIFO_RESULT for reading");
            continue;
        }

        int result;
        if (read(result_fd, &result, sizeof(result)) <= 0) {
            perror("Failed to read result from FIFO_RESULT");
        } else {
            printf("Result: %d\n", result);
        }
        close(result_fd);
    }

    // Wait for all child processes to finish
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // Cleaar and clean FIFO files
    unlink(FIFO_ADD);
    unlink(FIFO_SUB);
    unlink(FIFO_MUL);
    unlink(FIFO_DIV);
    unlink(FIFO_RESULT);

    return 0;
}

