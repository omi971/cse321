#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid_child = fork(); // create child process

    if (pid_child < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid_child == 0) {
        // Inside child process
        printf("2. Child process ID: %d\n", getpid());

        for (int i = 0; i < 3; i++) {
            pid_t pid_gc = fork();

            if (pid_gc < 0) {
                perror("Fork failed");
                return 1;
            }

            if (pid_gc == 0) {
                // Inside grandchild process
                printf("%d. Grandchild process ID: %d\n", i + 3, getpid());
                exit(0); // Exit grandchild so it doesn't create more children
            }

            // Child process continues to create next grandchild
        }

        // Wait to keep child alive while grandchildren print
        sleep(1);
    } else {
        // Inside parent process
        printf("1. Parent process ID: %d\n", getpid());
        sleep(2); // Wait so child and grandchild can finish
    }

    return 0;
}
