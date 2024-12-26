#include <stdio.h>
#include <stdlib.h>

// recieves from operation files and adds result to file

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Error: Incorrect number of arguments. Usage: %s <result>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *result = argv[1];
    FILE *file = fopen("results.txt", "a");
    if (file == NULL) {
        perror("Error opening results.txt");
        return EXIT_FAILURE;
    }

    fprintf(file, "%s\n", result);
    fclose(file);
    return EXIT_SUCCESS;



    const char *result =argv[1];
    FILE *file = fopen("results.txt","a");
    if(file==NULL){
perror("error opening results");
return EXIT_FAILURE;

    }

    
}

