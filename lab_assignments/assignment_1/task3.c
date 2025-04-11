// printf("------------ Task-3 ------------ ");
#include <stdio.h>
#include <string.h>

int main(){
    int a, b, c;
    a = fork();
    b = fork();
    c = fork();
    
    if (a%2 == 1){
        fork();
    }

    if (b%2 == 1){
        fork();
    }

    if (c%2 == 1){
        fork();
    }

    printf("PID: %d, PPID: %d\n", getpid(), getppid());
    return 0;
}