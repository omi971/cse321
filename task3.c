#include <stdio.h>
#include <string.h>

int main(){
    int radius;
    double circumference, area;

    radius = 9;

    circumference = 2 * 3.1416 * radius;
    area = 3.1416 * (radius * radius);

    printf("The Perimeter is: %lf\n", circumference);
    printf("The area is: %lf\n", area);

    return 0;
}