#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define NUM_TRIES (10)

// #undef COM_PREFIX
#ifndef COM_PREFIX
    #define COM_PREFIX "com"
#endif // !COM_PREFIX

#define SHM_PATH(s) ("/" COM_PREFIX "." s)
#define SEM_PATH(s) ("/" COM_PREFIX "." s)

#define SHARED_RES_PERMS (0666)
#define SHM_SIZE         (4096)
#define SHM_OFFSET       (0)

const char *shm_path = SHM_PATH("rsrc");
const char *sync_prod_path = SEM_PATH("prod");
const char *sync_cons_path = SEM_PATH("cons");
const char *sync_rsrc_path = SEM_PATH("rsrc");

typedef struct
{
    int shm_fd;
    void* shm_ptr_base;
    sem_t *sync_prod;
    sem_t *sync_cons;
    sem_t *sync_rsrc;
} shared_state_t;

void *producer(void *pv)
{
    shared_state_t *p_cs = (shared_state_t*)pv;
    const struct timespec sl_time = {
        .tv_sec = 0,
        .tv_nsec = 50 * 1000 * 1000 // 50ms
    };
    for (int i = 0; i < NUM_TRIES; i++)
    {
        // Sleep for a bit of time to simulate some work
        nanosleep(&sl_time, NULL);

#ifdef DEBUG
        printf("Wait for Consumer\n");
#endif // DEBUG

        // Wait until client(s) become(s) available
        sem_wait(p_cs->sync_cons);

        // Wait until resource is free
        sem_wait(p_cs->sync_rsrc);

        // Publish Resource
        *((int*)p_cs->shm_ptr_base) = i;
        printf("[producer] %d\n", i);

        // Release resource
        sem_post(p_cs->sync_rsrc);

#ifdef DEBUG
        printf("Available\n");
#endif // DEBUG

        // Signal to consumer that data is available
        sem_post(p_cs->sync_prod);
    }
    //pthread_exit(NULL);
}

int main()
{
    shared_state_t cs;
    int ret = 0;

#ifdef DEBUG
    printf("SHM_RSRC = %s\n", shm_path);
    printf("SEM_PROD = %s\n", sync_prod_path);
    printf("SEM_CONS = %s\n", sync_cons_path);
    printf("SEM_RSRC = %s\n", sync_rsrc_path);
#endif // DEBUG

    // Producer creates a semaphore for producer availability
    cs.sync_prod = sem_open(sync_prod_path, O_CREAT | O_RDWR, SHARED_RES_PERMS, 0);
    if (cs.sync_prod == SEM_FAILED)
    {
        perror("sem_open");
        ret = errno;
        goto main_end;
    }
    // Producer creates a semaphore for consumer availability
    cs.sync_cons = sem_open(sync_cons_path, O_CREAT | O_RDWR, SHARED_RES_PERMS, 0);
    if (cs.sync_cons == SEM_FAILED)
    {
        perror("sem_open");
        ret = errno;
        goto main_err_sync_cons;
    }
    // Producer creates a semaphore for resource access
    cs.sync_rsrc = sem_open(sync_rsrc_path, O_CREAT | O_RDWR, SHARED_RES_PERMS, 1);
    if (cs.sync_rsrc == SEM_FAILED)
    {
        perror("sem_open");
        ret = errno;
        goto main_err_sync_rsrc;
    }

    // Producer creates a shm
    cs.shm_fd = shm_open(shm_path, O_CREAT | O_RDWR, SHARED_RES_PERMS);
    if (cs.shm_fd < 0)
    {
        perror("shm_open");
        ret = errno;
        goto main_err_shm;
    }

    // Rezize shm accordingly
    ftruncate(cs.shm_fd, SHM_SIZE);
    // Mmap shm into memory
    cs.shm_ptr_base = mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, cs.shm_fd, SHM_OFFSET);
    if(cs.shm_ptr_base == MAP_FAILED)
    {
        perror("mmap");
        ret = errno;
        goto main_err_mmap;
    }

    printf("Starting Producer...\n");
    producer(&cs);

    // Munmap shm from memory
    ret = munmap(cs.shm_ptr_base, SHM_SIZE);
    if(ret < 0)
    {
        perror("munmap");
        goto main_err_mmap;
    }
main_err_mmap:
    // producer removes shm
    shm_unlink(shm_path);
main_err_shm:
    // Close resource semaphore
    sem_close(cs.sync_rsrc);
    // Producer Removes the semophore
    sem_unlink(sync_rsrc_path);
main_err_sync_rsrc:
    // Close consumer availability semaphore
    sem_close(cs.sync_cons);
    // Producer Removes the semophore
    sem_unlink(sync_cons_path);
main_err_sync_cons:
    // Close producer availability semaphore
    sem_close(cs.sync_prod);
    // Producer Removes the semophore
    sem_unlink(sync_prod_path);
main_end:
    return ret; //exit(0);
}
