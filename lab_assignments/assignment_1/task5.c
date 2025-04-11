// ------------------ Task-5  ------------------ 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
    int pid, pid2, status;
    
    pid = fork();

    if (pid < 0){
        printf("Fork Failed\n");
    }

    if (pid == 0){
        printf("2. Child Process ID: %d\n", getpid());
        for(int i = 0; i < 3; i++){
            pid2 = fork();

            if(pid2 < 0){
                printf("Fork Failed");
            }

            if(pid2 == 0){
                // Grandchild process
                printf("%d. Grandchiled Process ID: %d\n",i+3, getpid());
                exit(0);
            }
            else {
                wait(NULL);
            }
        }
    }
    else {
        // wait(NULL);
        // printf("--- Parent Process ---");
        printf("1. Parent Process ID: %d\n", getpid());
    }

    return 0;
}