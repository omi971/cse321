#include <stdio.h>
#include <string.h>

int main(){
    int num = 123;

    double hour, minutes, seconds;

    hour = num / 60;
    int hour;
    minutes = hour - (hour*60);
    seconds = minutes -(minutes*60);
    
    printf("The given number is: %d\n", num);
    printf("The given number is: %0.2lf\n", hour);
    printf("The given number is: %0.2lf\n", minutes);
    printf("The given number is: %0.2lf\n", seconds);

    return 0;
}