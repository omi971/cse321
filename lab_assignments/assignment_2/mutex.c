// Example of Mutex
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>


int count = 0;
int id[2] = {1, 2};
pthread_mutex_t mutex; // mutex variable (It should be global variable)

void *func(void* arg){
    int num = *(int*)arg;
    printf("Entered Thread: %d\n", num);
    
    for(int i = 0; i < 10000000; i++){
        pthread_mutex_lock(&mutex); // Lock the mutex before accessing the shared resource
        // Here if thread 1 lock the mutex then thread 2 can not access the count variable
        // Thread 1 have to unlock the mutex first then only thread 2 can access the count variable

        count++;
        pthread_mutex_unlock(&mutex); // Unlock the mutex after accessing the shared resource
    }
}


int main(){
    pthread_t t_id[2];
    pthread_mutex_init(&mutex, NULL); // Initialize the mutex
    // Paramenters for pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
    // mutex = NULL --> default attributes

    for(int i = 0; i < 2; i++){
        // printf("Thread ID: %d\n", id[i]);
        pthread_create(&t_id[i], NULL, func, (void*)&id[i]);
    }

    for(int i = 0; i < 2; i++){
        pthread_join(t_id[i], NULL);
    }

    pthread_mutex_destroy(&mutex); // Destroy the mutex after use
    printf("Total Count: %d\n", count);


    return 0;
}
