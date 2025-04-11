// ------------- Task - 1 ------------- 

#include<stdio.h>
// #include<fcntl.h>


int main() {
    int pid, pid2, status;
    pid = fork();

    if (pid < 0){
        printf("Fork Failed\n");
        exit(1);
    }
    
    if(pid == 0){
        // Child Process
        pid2 = fork();
        
        if (pid2 == 0){
            // Grandchild process
            printf("I am grandchild \n");
            exit(0);
            // wait(&status);
        }
        else {
            wait(&status);
            printf("I am child \n");
            exit(0);
        }
    }
    else {
        // parent process
        wait(&status);
        printf("I am parent \n");
    }
}