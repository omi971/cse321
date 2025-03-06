#include <stdio.h>
#include <string.h>

int main(){
    int height, width, perimeter, area;

    height = 7;
    width = 5;

    perimeter = 2 * (height + width);
    area = height * width;

    printf("The Perimeter is: %d\n", perimeter);
    printf("The area is: %d\n", area);

    return 0;
}