#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#define HISTORY_SIZE 50



int nothing = 0;
// ===========================================================
char* history[HISTORY_SIZE];
int history_count = 0;

// Add a command to history
void add_to_history(const char* cmd) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(cmd);
    } else {
        // Remove oldest and shift
        free(history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++) {
            history[i - 1] = history[i];
        }
        history[HISTORY_SIZE - 1] = strdup(cmd);
    }
}

// Display history
void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("[%d]: %s\n", i + 1, history[i]);
    }
}

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
        add_to_history(input);
        printf("You entered the string: %s\n", input); // Just for testing

        // Exiting function
        if(strcmp(input, "exit") == 0){
            printf("Exiting the shell\n");
            break;
        }

        if(strcmp(input, "history") == 0){
            show_history();
        }
    }



    return 0;
}