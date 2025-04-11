#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){
    // printf("------------ Task-4 ------------ ");
    int pid, status;
    char *array[] = {"X", "5", "3", "2", "6", "1", NULL}; // My decleared array

    // char *args[] = {"2", NULL};
    // execv("./ex2", args);


    pid = fork();
    if(pid == 0){
        // sort the array
        printf("Let's start sorting in child process\n");
        fflush(stdout);
        execv("./sort", array);

    }

    else {
        wait(&status);
        printf("\nNow lets sort the array in parent process\ns");
        fflush(stdout);
        execv("./oddeven", array);


    }
    return 0;
}