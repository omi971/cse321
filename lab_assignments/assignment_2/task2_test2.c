#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_STUDENTS 10
#define NUM_CHAIRS 3

sem_t students_waiting;
sem_t tutor_ready;
pthread_mutex_t chair_mutex;

int students_in_waiting_room = 0;
int students_served = 0;

void* student_thread(void* arg) {
    int id = *((int*)arg);
    sleep(rand() % 2);  // simulate arrival time

    while (1) {
        pthread_mutex_lock(&chair_mutex);
        if (students_in_waiting_room < NUM_CHAIRS) {
            students_in_waiting_room++;
            printf("[Student %d] Sitting in waiting room. Waiting students: %d\n", id, students_in_waiting_room);
            pthread_mutex_unlock(&chair_mutex);

            sem_post(&students_waiting);     // notify tutor
            sem_wait(&tutor_ready);          // wait to be called

            printf("[Student %d] Getting consultation...\n", id);
            sleep(1 + rand() % 2);           // consultation time
            printf("[Student %d] Done and leaving.\n", id);

            pthread_mutex_lock(&chair_mutex);
            students_served++;
            pthread_mutex_unlock(&chair_mutex);
            break;
        } else {
            pthread_mutex_unlock(&chair_mutex);
            printf("[Student %d] No chairs. Will retry soon.\n", id);
            usleep(150000);  // retry after 150ms
        }
    }

    pthread_exit(NULL);
}

void* tutor_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&chair_mutex);
        if (students_served >= NUM_STUDENTS) {
            pthread_mutex_unlock(&chair_mutex);
            break;
        }
        pthread_mutex_unlock(&chair_mutex);

        sem_wait(&students_waiting); // wait for a student

        pthread_mutex_lock(&chair_mutex);
        if (students_in_waiting_room > 0) {
            students_in_waiting_room--;
            printf("[Tutor] Calling in a student. Waiting students left: %d\n", students_in_waiting_room);
        }
        pthread_mutex_unlock(&chair_mutex);

        sem_post(&tutor_ready); // let student in
        sleep(1);               // simulate tutoring time
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t tutor;
    pthread_t students[NUM_STUDENTS];
    int ids[NUM_STUDENTS];

    sem_init(&students_waiting, 0, 0);
    sem_init(&tutor_ready, 0, 0);
    pthread_mutex_init(&chair_mutex, NULL);

    pthread_create(&tutor, NULL, tutor_thread, NULL);

    for (int i = 0; i < NUM_STUDENTS; ++i) {
        ids[i] = i + 1;
        pthread_create(&students[i], NULL, student_thread, &ids[i]);
    }

    for (int i = 0; i < NUM_STUDENTS; ++i) {
        pthread_join(students[i], NULL);
    }

    sem_post(&students_waiting);  // ensure tutor exits if waiting
    pthread_join(tutor, NULL);

    sem_destroy(&students_waiting);
    sem_destroy(&tutor_ready);
    pthread_mutex_destroy(&chair_mutex);

    printf("All students have been served. Program exiting.\n");
    return 0;
}