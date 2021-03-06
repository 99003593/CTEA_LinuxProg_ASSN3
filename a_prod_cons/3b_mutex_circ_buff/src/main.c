#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "cirque.h"

#define NUM_TRIES   (10)

cirque_t *p_crq;
sem_t sync_prodcons;
pthread_mutex_t mx_cirque;

void print_cirque(const char *tag, cirque_t *p_crq)
{
    int *p_i;

    printf("%s", tag);
    for (int i = 0; i < cirque_size(p_crq); i++)
    {
        p_i = (int *)cirque_peek(p_crq, i);
        if (p_i != NULL)
            printf(" %d", *(p_i));
        else
            printf(" %c", '-');
    }
    printf("\n");
}

void *producer(void *pv)
{
    const struct timespec sl_time = {
        .tv_sec = 0,
        .tv_nsec = 50 * 1000 * 1000 // 50ms
    };
    int *p_t;
    for (int i = 0; i < NUM_TRIES; i++)
    {
        // Sleep for a bit of time to simulate some work
        nanosleep(&sl_time, NULL);
        // Wait until resource is free
        pthread_mutex_lock(&mx_cirque);

        p_t = (int *)malloc(sizeof(int));
        *p_t = i;
        cirque_enqueue(p_crq, p_t);

        print_cirque("[producer]", p_crq);

        // Free resource
        pthread_mutex_unlock(&mx_cirque);
        // Signal to consumer that data is available
        sem_post(&sync_prodcons);
    }
    //pthread_exit(NULL);
}

void *consumer(void *pv)
{
    const struct timespec sl_time = {
        .tv_sec = 0,
        .tv_nsec = 380 * 1000 * 1000 // 380ms
    };
    int *p_t;
    for (int i = 1; i <= NUM_TRIES; i++)
    {
        // Wait until data is available from producer
        sem_wait(&sync_prodcons);
        // Wait until resource is free
        pthread_mutex_lock(&mx_cirque);

        p_t = (int *)cirque_dequeue(p_crq);

        print_cirque("<consumer>", p_crq);

        // Release data memory
        free(p_t);

        // Free resource
        pthread_mutex_unlock(&mx_cirque);
        // Sleep for a bit of time to simulate some work
        nanosleep(&sl_time, NULL);
    }
    //pthread_exit(NULL);
}
int main()
{
    pthread_t th_p, th_c;
    // Initialize semaphore for producer-consumer syncronization
    sem_init(&sync_prodcons, 0, 0);
    // Initialize semaphore for resource access
    pthread_mutex_init(&mx_cirque, NULL);

    // Initialize cirque
    p_crq = cirque_create(NUM_TRIES);

    pthread_create(&th_p, NULL, producer, NULL);
    pthread_create(&th_c, NULL, consumer, NULL);
    pthread_join(th_p, NULL);
    pthread_join(th_c, NULL);

    sem_destroy(&sync_prodcons);
    pthread_mutex_destroy(&mx_cirque);
    cirque_destroy(p_crq);
    return 0; //exit(0);
}
