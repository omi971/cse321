#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

int count = 0;
int id[2] = {1, 2};

// This is not atomic operation
void *func(void* arg){
    int num = *(int*)arg;

    printf("Entered Thread: %d\n", num);
    // sleep(1);
    for(int i = 0; i < 100000; i++){
        count++;
    }
}

// read count from memory
// update count
// write count to memory

int main(){
    pthread_t t_id[2];

    for(int i = 0; i < 2; i++){
        // printf("Thread ID: %d\n", id[i]);
        pthread_create(&t_id[i], NULL, func, (void*)&id[i]);
    }

    for(int i = 0; i < 2; i++){
        pthread_join(t_id[i], NULL);
    }


    printf("Total Count: %d\n", count);
    return 0;
}
