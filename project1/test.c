#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#define HISTORY_SIZE 50
#define BUFFER_SIZE 1024



int nothing = 0;
char* history[HISTORY_SIZE];
int history_count = 0;


// ======================[Task 7]======================
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
    return;
}

// Display history
void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("[%d]: %s\n", i + 1, history[i]);
    }
    return; // Ensure no further execution after handling 'history'

}
// ======================[Task 7]======================


// ======================[Task 1 & 2]======================
pid_t activeChildPid = -1;
// #2 Parsing user input
void parseCommandInput(char *user_input, char **marks) {
    if (!user_input) {
        marks[0] = NULL;
        return;
    }

    int indx = 0;
    marks[indx] = strtok(user_input, " \t\n");
    for (; marks[indx] != NULL; indx++) {
        marks[indx + 1] = strtok(NULL, " \t\n");
    }
    marks[indx] = NULL;
}

void executeSingleCommand(char **tokens) {
    if (tokens[0] == NULL) return;

    if (strcmp(tokens[0], "cd") == 0) {
        if (tokens[1]) {
            if (chdir(tokens[1]) != 0)
                perror("cd");
        } else {
            chdir(getenv("HOME"));
        }
        return;
    }

    // Exiting function
    if(strcmp(tokens[0], "exit") == 0){
        printf("Exiting the shell\n");
        exit(0);
        // return;
    }

    if(strcmp(tokens[0], "history") == 0){
        show_history();
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        execvp(tokens[0], tokens);
        perror("exec failed");
        exit(1);
    } else {
        activeChildPid = pid;
        wait(NULL);
        activeChildPid = -1;
    }
}
// ======================[Task 1 & 2]======================




// ======================[Task 8]======================
// Handle Ctrl+C (SIGINT)
void handle_sigint(int sig) {
    printf("\nCaught Ctrl+C (SIGINT).\nExiting the shell.\n");
    exit(0);
}
// ======================[Task 8]======================

// ======================[Additional]======================
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
// ======================[Additional]======================


int main(){
    printf("--------------------------------------\n");
    printf("======================================\n");
    printf("--------------------------------------\n");
    printf("Welcome to the CSE321 Project Shell\n\n");

    signal(SIGINT, handle_sigint);  // Register Ctrl+C handler
    char input[MAX_INPUT];
    char dynamicBuffer[BUFFER_SIZE];
    char *tokens[BUFFER_SIZE / 2]; // enough space for tokens

    while (1){
        char *dir = get_directory();
        printf("%s$ ", dir);
        fflush(stdout);

        if (fgets(dynamicBuffer, sizeof(dynamicBuffer), stdin) == NULL) {
            if (feof(stdin)) {
                printf("\nExiting shell...\n");
                break;
            } else {
                perror("fgets");
                exit(EXIT_FAILURE);
            }
        }

        dynamicBuffer[strcspn(dynamicBuffer, "\n")] = '\0';
        printf("You entered the string: %s\n", dynamicBuffer); // Just for testing
        add_to_history(dynamicBuffer);

        if (strlen(dynamicBuffer) == sizeof(dynamicBuffer) - 1 && dynamicBuffer[sizeof(dynamicBuffer) - 2] != '\n') {
            fprintf(stderr, "Warning: Input truncated. Maximum %zu characters allowed.\n", sizeof(dynamicBuffer) - 1);
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        if (strlen(dynamicBuffer) == 0) {
            continue;
        }

        parseCommandInput(dynamicBuffer, tokens);
        executeSingleCommand(tokens);
    }

    return 0;
}