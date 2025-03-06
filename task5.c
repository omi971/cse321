#include <string.h>
#include <stdio.h>

int main(){
    int arr[10] = {1, 2, 3, 4, 5, 60, 70, 80, 90, 100};

    for (int i = 9; i >= 0; i--){
        printf("The arr[%d] element of the array is: %d\n", i,  arr[i]);
    }
    return 0;
}