#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>



#define HISTORY_SIZE 50 // Maximum size of the history it can hold
#define BUFFER_SIZE 1024
int nothing = 0;

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

    // ------------------- Exiting function feature ---------------------
    if(strcmp(tokens[0], "exit") == 0){
        printf("Exiting the shell\n");
        exit(0);
        // return;
    }

    // ------------------- History feature ---------------------
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

// ======================[Task 4 & 5 & 6]======================
void handle_multiple_commands(char *input, char **tokens) {
    char *semicolon_cmds[BUFFER_SIZE];
    int sc_count = 0;

    char *temp = strdup(input);
    char *token = strtok(temp, ";");
    while (token != NULL && sc_count < BUFFER_SIZE) {
        semicolon_cmds[sc_count++] = strdup(token);
        token = strtok(NULL, ";");
    }

    for (int i = 0; i < sc_count; i++) {
        char *and_cmds[BUFFER_SIZE];
        int ac_count = 0;

        token = strtok(semicolon_cmds[i], "&&");
        while (token != NULL && ac_count < BUFFER_SIZE) {
            and_cmds[ac_count++] = token;
            token = strtok(NULL, "&&");
        }

        int run_next = 1;
        for (int j = 0; j < ac_count; j++) {
            while (*and_cmds[j] == ' ') and_cmds[j]++;
            char *end = and_cmds[j] + strlen(and_cmds[j]) - 1;
            while (end > and_cmds[j] && (*end == ' ')) *end-- = '\0';

            if (!run_next) break;

            if (strchr(and_cmds[j], '|')) {
                executePipedCommands(and_cmds[j]);
                run_next = 1;
            } else {
                parseCommandInput(and_cmds[j], tokens);
                if (tokens[0] == NULL) continue;

                if (strcmp(tokens[0], "cd") == 0) {
                    if (tokens[1]) {
                        if (chdir(tokens[1]) != 0)
                            perror("cd");
                    } else {
                        chdir(getenv("HOME"));
                    }
                    run_next = 1;
                    continue;
                }

                if (strcmp(tokens[0], "exit") == 0) {
                    printf("Exiting the shell\n");
                    exit(0);
                }

                if (strcmp(tokens[0], "history") == 0) {
                    show_history();
                    run_next = 1;
                    continue;
                }

                pid_t pid = fork();
                if (pid == 0) {
                    execvp(tokens[0], tokens);
                    perror("exec failed");
                    exit(1);
                } else {
                    int status;
                    waitpid(pid, &status, 0);
                    run_next = WIFEXITED(status) && WEXITSTATUS(status) == 0;
                }
            }
        }

        // Free command segments
        free(semicolon_cmds[i]);
    }
    free(temp);
}
// ======================[Task 4 & 5 & 6]======================



// ======================[Task 4 - Command piping]======================
void executePipedCommands(char *input) {
    char *commands[BUFFER_SIZE];
    int num_cmds = 0;

    // Split by pipe symbol
    char *cmd = strtok(input, "|");
    while (cmd != NULL) {
        commands[num_cmds++] = strdup(cmd);
        cmd = strtok(NULL, "|");
    }

    int pipefd[2], prev_fd = 0;

    for (int i = 0; i < num_cmds; i++) {
        pipe(pipefd);

        pid_t pid = fork();
        if (pid == 0) {
            // Child
            if (i != 0) {
                dup2(prev_fd, 0); // stdin from previous pipe
                close(prev_fd);
            }
            if (i != num_cmds - 1) {
                dup2(pipefd[1], 1); // stdout to next pipe
            }
            close(pipefd[0]); // Close unused read end
            close(pipefd[1]);

            // Tokenize individual command
            char *argv[BUFFER_SIZE / 2];
            parseCommandInput(commands[i], argv);

            execvp(argv[0], argv);
            perror("execvp failed");
            exit(1);
        } else {
            // Parent
            wait(NULL);
            close(pipefd[1]); // Close write end
            if (i != 0) close(prev_fd); // Close old read end
            prev_fd = pipefd[0]; // Save new read end
        }
    }
}
// ======================[Task 4 - Command piping]======================


// ======================[Task 7 - History]======================
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
    return;
}

// Display history
void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("[%d]: %s\n", i + 1, history[i]);
    }
    return;
}
// ======================[Task 7]======================



// ======================[Task 8] ======================
void handle_sigint(int sig) {
    printf("\nGot keyboard interrupt Ctrl+C.\nExiting the shell.\n");
    exit(0);
}
// ======================[Task 8] ======================



// ======================[Additional- function like linux terminal]======================
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
        add_to_history(dynamicBuffer); // adding all input to the history array

        if (strlen(dynamicBuffer) == sizeof(dynamicBuffer) - 1 && dynamicBuffer[sizeof(dynamicBuffer) - 2] != '\n') {
            fprintf(stderr, "Warning: Input truncated. Maximum %zu characters allowed.\n", sizeof(dynamicBuffer) - 1);
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        if (strlen(dynamicBuffer) == 0) {
            continue;
        }
        handle_multiple_commands(dynamicBuffer, tokens);
    }

    return 0;
}