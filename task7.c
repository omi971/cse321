#include <stdio.h>
#include <string.h>

int main(){
    int a, b, temp;

    a = 69;
    b = 96;

    printf("The Value of a: %d\n", a);
    printf("The Value of a: %d\n", b);

    temp = a;
    a = b;
    b = temp;

    printf("\n\nThe Value of a: %d\n", a);
    printf("The Value of b: %d\n", b);
    return 0;
}