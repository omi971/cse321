// Argument Passing in thread
#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // for sleep function

int arr[2] = {10, 20};
void *add(void* arg){
    int *value = arg;
    int sum = value[0] + value[1];
    sleep(1);
    printf("The sum is: %d\n", sum);

}

void *mul(void* arg){
    int *value = arg;
    int mul = value[0] * value[1];
    sleep(1);
    printf("The multiplication is: %d\n", mul);
}


int main(){
    pthread_t thread1, thread2; // three thread using array
    // 1st_param --> thread initialization
    // 3rd_param --> thread will be run on that function
    // 4rd_param --> argument passing on the 3rd param function

    pthread_create(&thread1, NULL, add, (void*)arr);
    pthread_create(&thread2, NULL, mul, (void*)arr);

    pthread_join(thread1, NULL); // waits for the thread1 to finish        
    pthread_join(thread2, NULL); // waits for the thread1 to finish        
    return 0;
}
