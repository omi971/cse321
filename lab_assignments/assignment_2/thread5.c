// Dynamic Memory Allocation stores the data in heap instead of memory
// for using the global variable instead of local
#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // for sleep function
#include <stdlib.h> // for dynamic memory allocation

void *compute_square(void* arg){
    int number = *(int*)arg;
    int* result = malloc(sizeof(int)); // Allocation a dynamic memory of the size of int

    if(result == NULL){
        perror("Failed to allocate memory");
        pthread_exit(NULL);
    }
    *result = number * number;
    printf("Thread Computed square of %d is %d\n", number, *result);
    pthread_exit(result);
}


int main(){
    pthread_t thread; // five thread using array
    int num = 6;

    pthread_create(&thread, NULL, compute_square, (void*)&num);

    void* thread_result;
    pthread_join(thread, &thread_result); // waits for the thread1 to finish
    // 2nd param --> used for dynamic memory allocation
    // This will take the return value from the thread function
    // in this case line: 18 "pthread_exit(result); --> result goes to the thread_result variable"

    int* square = (int*)thread_result;
    printf("Received square %d\n", *square);
    free(square);


    return 0;
}
