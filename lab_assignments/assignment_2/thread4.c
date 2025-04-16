// Variable sharing in thread
#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // for sleep function

int t_id[] = {1, 2, 3, 4, 5};

void *func(void* arg){
    int *id = arg;
    printf("Entered Thread: %d\n", *id);
    for(int i = 0; i < 5; i++){
        printf("Thread %d turn %d\n", *id, i);
        sleep(1);
    }
    printf("Ending thread%d\n", *id);


}


int main(){
    pthread_t thread[5]; // five thread using array
    for(int i = 0; i < 5; i++){
        pthread_create(&thread[i], NULL, func, (void*)&t_id[i]);
        sleep(1);
    }

    for(int i = 0; i < 5; i++){
        pthread_join(thread[i], NULL); // waits for the thread1 to finish        
        // printf("Thread: %d has finished running\n", i);
    }
        // pthread_join(thread[2], NULL); // waits for the thread1 to finish        


    return 0;
}
