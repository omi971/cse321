#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


int nothing = 0;

// Handle Ctrl+C (SIGINT)
void handle_sigint(int sig) {
    printf("\nCaught Ctrl+C (SIGINT).\nExiting the shell.\n");
    exit(0);
}

char* get_directory() {
    char *cwd = malloc(PATH_MAX);  // allocate memory
    if (cwd == NULL) {
        perror("malloc() failed");
        exit(1);
    }

    if (getcwd(cwd, PATH_MAX) != NULL) {
        return cwd;
    } else {
        perror("getcwd() error");
        free(cwd);
        return NULL;
    }
}

int main(){
    printf("--------------------------------------\n");
    printf("======================================\n");
    printf("--------------------------------------\n");
    printf("Welcome to the CSE31 Project Shell\n\n");
    
    signal(SIGINT, handle_sigint);  // Register Ctrl+C handler

    char input[MAX_INPUT];

    while (1){
        char *dir = get_directory();
        printf("%s$ ", dir);

        // Read input including spaces
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // If input failed (EOF or error), continue
            continue;
        }
        input[strcspn(input, "\n")] = '\0';
        printf("You entered the string: %s\n", input); // Just for testing

        // Exiting function
        if(strcmp(input, "exit") == 0){
            printf("Exiting the shell\n");
            break;
        }
    }



    return 0;
}