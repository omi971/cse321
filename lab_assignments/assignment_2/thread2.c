// Variable sharing in thread
#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // for sleep function

int var = 0;
void *block(void* arg){
    var++;
}


int main(){
    pthread_t thread[3]; // three thread using array
    for(int i = 0; i < 3; i++){
        // 1st_param --> thread initialization
        // 3rd_param --> thread will be run on that function
        pthread_create(&thread[i], NULL, block, NULL);
        printf("Thread: %d has started running\n", i);
        sleep(1);
        
        // // if we use join here
        // // here thread used but the code work as a sequencial code
        // pthread_join(thread[i], NULL); // waits for the thread1 to finish        
        // printf("Thread: %d has finished running\n", i);
    }


    for(int i = 0; i < 3; i++){
        pthread_join(thread[i], NULL); // waits for the thread1 to finish        
        printf("Thread: %d has finished running\n", i);
    }
        // pthread_join(thread[2], NULL); // waits for the thread1 to finish        


    printf("All thread has finished and the var is: %d\n", var);
    return 0;
}
