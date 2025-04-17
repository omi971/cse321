#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int count = 0;
int t_arg[2] = {1, 2};
pthread_mutex_t lock;  // mutex lock

void *func(void* arg){
    int num = *(int*)arg;

    for(int i = 0; i < 100000; i++){
        pthread_mutex_lock(&lock);
        count++;
        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}

int main(){
    pthread_t t_id[2];
    pthread_mutex_init(&lock, NULL);  // initialize mutex

    for(int i = 0; i < 2; i++){
        pthread_create(&t_id[i], NULL, func, (void*)&t_arg[i]);
    }

    for(int i = 0; i < 2; i++){
        pthread_join(t_id[i], NULL);
    }

    pthread_mutex_destroy(&lock); // destroy mutex
    printf("Total Count: %d\n", count);
    return 0;
}
