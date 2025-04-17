#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <linux/limits.h>

#define MAX_INPUT 1000

// Handle Ctrl+C (SIGINT)
void handle_sigint(int sig) {
    printf("\nCaught Ctrl+C (SIGINT). Exiting the shell.\n");
    exit(0);
}

// Get current working directory
char* get_directory() {
    char *cwd = malloc(PATH_MAX);
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

int main() {
    signal(SIGINT, handle_sigint);  // Register Ctrl+C handler

    char input[MAX_INPUT];

    printf("--------------------------------------\n");
    printf("======================================\n");
    printf("--------------------------------------\n");
    printf("Welcome to the CSE31 Project Shell\n\n");

    while (1) {
        char *dir = get_directory();
        if (dir) {
            printf("%s$ ", dir);
            free(dir);
        }

        if (fgets(input, sizeof(input), stdin) == NULL) {
            continue;
        }
        input[strcspn(input, "\n")] = '\0'; 

        if (strcmp(input, "exit") == 0) {
            printf("Exiting the shell\n");
            break;
        }

        // Debug: echo the input
        printf("You entered the string: %s\n", input);
    }

    return 0;
}
