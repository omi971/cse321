// Dynamic Memory Allocation stores the data in heap instead of stack
// Using dynamic memory to return values from thread safely

#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // for sleep function
#include <stdlib.h> // for dynamic memory allocation

void *compute_square(void* arg){
    int number = *(int*)arg;

    int* result = malloc(sizeof(int));
    if(result == NULL){
        perror("Failed to allocate memory");
        pthread_exit(NULL);  // exit if malloc fails
    }

    *result = number * number;
    printf("Thread computed square of %d is %d\n", number, *result);

    pthread_exit(result);  // return the pointer to the result
}

int main(){
    pthread_t thread;
    int num = 6;

    // Start thread and pass pointer to `num`
    pthread_create(&thread, NULL, compute_square, (void*)&num);

    void* thread_result;
    pthread_join(thread, &thread_result);  // collect result from thread

    int* square = (int*)thread_result;
    if (square != NULL) {
        printf("Received square: %d\n", *square);
        free(square);  // always free malloc'ed memory
    }

    return 0;
}
