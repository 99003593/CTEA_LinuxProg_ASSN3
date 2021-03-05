#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

#define NUM_TRIES (10)

stack_t *p_stk;
sem_t sync_prodcons;
sem_t sync_resource;

void print_stack(const char *tag, stack_t *p_stk)
{
    printf("%s", tag);
    for (int i = 0; i < stack_size(p_stk); i++)
    {
        printf(" %d", *((int*)stack_peek(p_stk, i)));
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
        sem_wait(&sync_resource);

        p_t = (int *)malloc(sizeof(int));
        *p_t = i;
        stack_push(p_stk, p_t);

        print_stack("[producer]", p_stk);

        // Free resource
        sem_post(&sync_resource);
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
        sem_wait(&sync_resource);

        p_t = (int *)stack_pop(p_stk);

        print_stack("<consumer>", p_stk);

        // Release data memory
        free(p_t);

        // Free resource
        sem_post(&sync_resource);
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
    sem_init(&sync_resource, 0, 1);

    // Initialize stack
    p_stk = stack_create(NUM_TRIES);

    pthread_create(&th_p, NULL, producer, NULL);
    pthread_create(&th_c, NULL, consumer, NULL);
    pthread_join(th_p, NULL);
    pthread_join(th_c, NULL);

    sem_destroy(&sync_prodcons);
    sem_destroy(&sync_resource);
    stack_destroy(p_stk);
    return 0; //exit(0);
}
