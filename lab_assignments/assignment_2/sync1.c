#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>


int count = 0;
int id[2] = {1, 2};
sem_t s; // semaphore variable (It should be global variable)
void *func(void* arg){
    int num = *(int*)arg;
    printf("Entered Thread: %d\n", num);
    
    for(int i = 0; i < 10000000; i++){
        sem_wait(&s); // wait for the semaphore, Here the value will be 0, So IF thread 2 comes in it can not access the 
        // count variable, since semaphore vallue is 0. It is stuck in a while loop busy waiting. 
        // When thread 1 calls post only then thread 2 come and access the count variable
        count++;
        sem_post(&s); // signal the semaphore, Here the value will be 1
    }
}


int main(){
    pthread_t t_id[2];
    sem_init(&s, 0, 1); // Paramenters for sem_init(sem_t *sem, int pshared, unsigned int value)
    //called the semaphore variable
    //pshared = 0 --> semaphore is shared between threads of the process
    //pshared = 1 --> semaphore is shared between processes 
    //value = 1 --> semaphore is initialized to 1
    //value = 0 --> semaphore is initialized to 0

    for(int i = 0; i < 2; i++){
        // printf("Thread ID: %d\n", id[i]);
        pthread_create(&t_id[i], NULL, func, (void*)&id[i]);
    }

    for(int i = 0; i < 2; i++){
        pthread_join(t_id[i], NULL);
    }

    sem_destroy(&s); // Destroy the semaphore after use
    printf("Total Count: %d\n", count);
    return 0;
}
