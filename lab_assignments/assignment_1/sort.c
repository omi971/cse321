#include <stdio.h>
#include <stdlib.h>

//  to test this type below 
//  ./test 3 4 5
int compare(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

void sort(int arr[], int size) {
    qsort(arr, size, sizeof(int), compare);
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

    // -------- To Sort
    int size = sizeof(array) / sizeof(array[0]);
    sort(array, size);

    // -------- Verify After sort
    printf("Sorted: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}