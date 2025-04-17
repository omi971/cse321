#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

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

int main() {
    char dynamicBuffer[BUFFER_SIZE];
    char *tokens[BUFFER_SIZE / 2]; // enough space for tokens

    for (;;) {
        printf("shell -> ");
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