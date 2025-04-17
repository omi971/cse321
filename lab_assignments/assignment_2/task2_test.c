#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t s_wait;
sem_t st_ready;
pthread_mutex_t mutex;

int total_s = 10; // total student number
int total_c = 3; // total available chairs number
int wait_s = 0; // student waiting number
int served = 0; // student server so far


// thread for the student
void *thread_s(void *arg) {
    int id = *(int*)arg;

    while (1 == 1) {
        pthread_mutex_lock(&mutex);
        if (wait_s < total_c) {
            printf("Student %d started waiting for consultation\n", id);
            wait_s= wait_s + 1;
            pthread_mutex_unlock(&mutex);
            sem_post(&s_wait);
            sem_wait(&st_ready);
            printf("Student %d is getting consultation\n", id);
            printf("Student %d finished getting consultation and left\n", id);
            pthread_mutex_lock(&mutex);
            served++;
            printf("Number of served students: %d\n", served);
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex); // else it will do this
    }
    pthread_exit(NULL); // thread exited
}

// Thread for st
void *thread_t(void *arg) {
    while (1 == 1) {
        pthread_mutex_lock(&mutex);
        if (served >= total_s) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        sem_wait(&s_wait);
        pthread_mutex_lock(&mutex);
        if (wait_s > 0) {
            wait_s= wait_s - 1;
            printf("A waiting student started getting consultation\n");
            printf("Number of students now waiting: %d\n", wait_s);
            printf("ST giving consultation\n");
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&st_ready);
    }
    pthread_exit(NULL);
}


int main() {
    pthread_t tutor;
    pthread_t students[total_s]; // array of student threads
    int id_[total_s];

    sem_init(&s_wait, 0, 0);
    sem_init(&st_ready, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&tutor, NULL, thread_t, NULL);

    for (int i = 0; i < total_s; i++) {
        id_[i] = i;
        pthread_create(&students[i], NULL, thread_s, &id_[i]);
    }

    for (int j = 0; j < total_s; j++) {
        pthread_join(students[j], NULL);
    }

    // Exiting everything
    sem_post(&s_wait);
    sem_destroy(&st_ready);
    sem_destroy(&s_wait);
    pthread_join(tutor, NULL);
    pthread_mutex_destroy(&mutex);

    return 0;
}
