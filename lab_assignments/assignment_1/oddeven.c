#include <stdio.h>
#include <stdlib.h>

void oddevencheck(int arr[], int size) {
    for(int i = 0; i < size; i++) {
        if (arr[i]%2 == 1){
            printf("The number %d, is odd\n", arr[i]);
        }
        else{
            printf("The number %d, is even\n", arr[i]);
        }
    }
}

int main(int argc, char *argv[]) {

    int count = argc - 1;
    int array[count];

    for (int i = 0; i < count; i++) {
        array[i] = atoi(argv[i + 1]);
    }

    // ------- To Verify
    printf("The numbers entered: ");
    for (int i = 0; i < count; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    // oddeven checker function
    int size = sizeof(array) / sizeof(array[0]);
    oddevencheck(array, size);


    return 0;
}