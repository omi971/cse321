#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
    // printf("Test---------------, %s", args[0]);
    printf("PID of ex1.c = %d\n", getpid());
    char *args[] = {"Hello", NULL};
    execv("./ex2", args);
    printf("Back to ex1.c");
    return 0;
}