
// Sleeping ST Problem
// Mainly solving of task 2
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>


// int count = 0;
int students_max = 10; // total student
int chair_max = 3; // total number of chair
int waiting = 0; // Student waiting for consultation
int served_student = 0; // total number of student left after getting consultation
int now = 0; // st teaching or not?
// int id[2] = {1, 2};
sem_t s; // semaphore for wait student
pthread_mutex_t mutex; // for consultation st
// pthread_mutex_t mutex1; // for consultation st else condition



void *consult_(void* arg){
    int stu_no = *(int*)arg;

    sem_wait(&s);
    if (waiting < 3){
        sleep(1);
        printf("Student %d started waiting for consultation\n", stu_no);
        waiting++;
        sem_post(&s);
        // ----------- Consultation Going On -----------------
        pthread_mutex_lock(&mutex);
        printf("A waiting student started getting consultation\n");
        waiting--;
        printf("Number of student now waiting: %d\n", waiting);
        printf("ST giving consultation\n");
        printf("Student %d getting consultation\n", stu_no);
        served_student++;
        sleep(1);
        pthread_mutex_unlock(&mutex);
        printf("Studet %d finished getting consultation and left\n", stu_no);
        printf("Number of served students: %d\n", served_student);
        // pthread_exit(NULL);
        // ------------------- Consultation Done -------------------
    }

    else {
        printf("No chairs remaining in lobby. Student %d leaving...\n", stu_no);
        sleep(1);
        served_student++;
        sem_post(&s);
        pthread_exit(NULL);
    }
}


int main(){
    pthread_t student[students_max];    
    sem_init(&s, 0, chair_max); // student waiting
    pthread_mutex_init(&mutex, NULL); // Initialize the mutex for st
    // pthread_mutex_init(&mutex1, NULL); // Initialize the mutex for st
    


    for(int i = 0; i < students_max; i++){
        int *stu_id = malloc(sizeof(int)); // allocate separate memory for each thread
        *stu_id = i;
        pthread_create(&student[i], NULL, consult_, stu_id);
    }

    for(int i = 0; i < students_max; i++){
        pthread_join(student[i], NULL);
    }

    sem_destroy(&s);
    pthread_mutex_destroy(&mutex);
    return 0;
}
