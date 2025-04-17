// Assignment-2 Task 1
#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // for sleep function
#include <stdlib.h> // for dynamic memory allocation

int arr_length = 0;
void *fibo_seq(void* arg){
    int fibo = 0;
    int prev = 1;

    int n = *(int*)arg;
    int* fibo_arr = malloc((n+1) * sizeof(int));

    if(fibo_arr == NULL){
        perror("Failed to allocate memory");
        pthread_exit(NULL);
    }


    printf("Fibonacci Sequence Generation\n");
    
    if(n >= 0){
        fibo_arr[0] = 0;
    }

    if(n >= 1){
        fibo_arr[1] = 1;
    }
    

    for(int i = 2; i <= n; i++){
        fibo_arr[i] = fibo_arr[i-1] + fibo_arr[i-2];
        printf("a[%d]=%d\n", i, fibo_arr[i]);
    }


    // // Just for checking
    // for(int i = 0; i <=n; i++){
    //     printf("Fibonacci number checking for %d'th term: %d\n", i, fibo_arr[i]);
    // }
    // *result = fibo_arr;
    printf("Done..\n");
    pthread_exit(fibo_arr);
}

void *fibo_search(void* arg){
    printf("Searching...");

    int t_num, n;
    int* arr = arg; // The full array
    
    printf("How many number you are willing to search?:\n");
    scanf("%d", &t_num);

    for(int i = 0; i < t_num; i++){
        printf("Enter Search %d:\n", i);
        scanf("%d", &n);

        if(n == 0){
            printf("result of search #0 = 0\n");
        }
        // arr[n];
        else if(n > 0 && n < arr_length){
            printf("result of search #%d = %d\n", n, arr[n]);
        }
        else {
            printf("result of search #%d = -1\n", n);
        }

    }
    

}

int main(){
    int term, t_num;
    
    printf("Enter the term of fibonacci sequence: ");
    scanf("%d", &term);
    arr_length = term; // updating the array length
    
    
    pthread_t thread, thread_s; // five thread using array
    pthread_create(&thread, NULL, fibo_seq, (void*)&term);
    
    void* thread_result;
    pthread_join(thread, &thread_result);
    
    int* fibo_seq = (int*)thread_result;
    printf("The fibonacci series from thread is: \n");
    // Just for checking
    for(int i = 0; i <=term; i++){
        printf("Fibonacci number checking for %d'th term: %d\n", i, fibo_seq[i]);
    }

    pthread_create(&thread_s, NULL, fibo_search, (void*)fibo_seq);
    
    void* thread_s_result;
    pthread_join(thread_s, &thread_s_result);
    free(fibo_seq);
    // free(fibo_search)


    return 0;
}
