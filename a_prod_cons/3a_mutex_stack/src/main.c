#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

#define NUM_TRIES (10)

// Thread safe stack using mutexes where appropriate
stack_t *p_stk;
sem_t sync_prodcons;
pthread_mutex_t mx_stack;

void print_stack(const char *tag, stack_t *p_stk)
{
    int* p_i;

    printf("%s", tag);
    for (int i = 0; i < stack_size(p_stk); i++)
    {
        p_i = (int*)stack_peek(p_stk, i);
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
        pthread_mutex_lock(&mx_stack);

        p_t = (int *)malloc(sizeof(int));
        *p_t = i;
        stack_push(p_stk, p_t);

        print_stack("[producer]", p_stk);

        // Free resource
        pthread_mutex_unlock(&mx_stack);
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
        pthread_mutex_lock(&mx_stack);

        p_t = (int *)stack_pop(p_stk);

        print_stack("<consumer>", p_stk);

        // Release data memory
        free(p_t);

        // Free resource
        pthread_mutex_unlock(&mx_stack);
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
    pthread_mutex_init(&mx_stack, NULL);

    // Initialize stack
    p_stk = stack_create(NUM_TRIES);

    pthread_create(&th_p, NULL, producer, NULL);
    pthread_create(&th_c, NULL, consumer, NULL);
    pthread_join(th_p, NULL);
    pthread_join(th_c, NULL);

    sem_destroy(&sync_prodcons);
    pthread_mutex_destroy(&mx_stack);
    stack_destroy(p_stk);
    return 0; //exit(0);
}
