#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // for sleep function

void *block(void* arg){
    for(int i = 1; i <= 5; i++){
        sleep(1);
        printf("%d. I am thread\n", i);
    }
}

void default_thread() {
    for(int i = 1; i <=3; i++){
        sleep(1);
        printf("%d. I am default thread\n", i);
    }
}

int main(){

    pthread_t thread1;
    pthread_create(&thread1, NULL, block, NULL);
    // 1st_param --> thread initialization
    // 3rd_param --> thread will be run on that function
    default_thread();
    pthread_join(thread1, NULL); // waits for the thread1 to finish


    return 0;
}
