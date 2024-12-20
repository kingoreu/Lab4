/* hellothreads.c */
/* pthread argument passing example */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_THREADS 3

void *hello_thread (void *arg)
{
    long thread_id = (long) arg;
    printf("Thread %ld: Hello, World!\n", thread_id);
    return NULL;
}

int main()
{
    pthread_t tid[NUM_THREADS];
    int status;

    /* 쓰레드 생성 */
    for (long i = 0; i < NUM_THREADS; i++) {
        status = pthread_create (&tid[i], NULL,
            hello_thread, (void*) i);
        if (status != 0) {
            fprintf (stderr, "Create thread %ld: %d", i, status);
            exit (1);
        }
    }
    pthread_exit (NULL);
}