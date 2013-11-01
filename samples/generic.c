#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 4

#include "utils.h"
#include "lock_if.h"


/* global data */
lock_global_data the_lock;

void *do_something(void *id)
{
    int* my_core = (int*) id; 
    /* local data */
    lock_local_data my_data;
    /*initialize this thread's local data*/
    init_lock_local(*my_core, &the_lock, &my_data);
    MEM_BARRIER;


    /*acquire the lock*/
    acquire_lock(&my_data,&the_lock);
    printf("I have the lock\n");
    /*release the lock*/
    release_lock(&my_data,&the_lock);


    MEM_BARRIER;
    /*free internal memory structures which may have been allocated for the local data*/
    free_lock_local(my_data);

    return NULL;

}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    long t;

    /*initialize the global data*/
    init_lock_global(&the_lock); 
    int ids[]={0,1,2,3};

    MEM_BARRIER;

    for(t=0;t<NUM_THREADS;t++){
        printf("In main: creating thread %ld\n", t);
        if (pthread_create(&threads[t], NULL, *do_something, &ids[t])!=0){
            fprintf(stderr,"Error creating thread\n");
            exit(-1);
        }
    }

    for (t = 0; t < NUM_THREADS; t++) {
        if (pthread_join(threads[t], NULL) != 0) {
            fprintf(stderr, "Error waiting for thread completion\n");
            exit(1);
        }
    }

    /*free internal memory strucutres which may have been allocated for this lock */
    free_lock_global(the_lock);

    pthread_exit(NULL);
}