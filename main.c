#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "prod_cons_MT.h"


int main(int argc, char** argv)
{
    uint32_t id;
    uint32_t buff_size;
    uint32_t num_producers;
    uint32_t num_consumers;
    uint32_t num_threads;
    void* thread_func;
    Queue q;

    printf("MAIN\n");

    // Seed random
    srand((unsigned int)time(0));

    // Check number of arguments
    if(argc != 4)
    {
        printf("USAGE: %s <buffer size> <#producers> <#consumers>\n", argv[0]);
        return 1;
    }

    // Parse the arguments
    buff_size = (uint32_t)atoi(argv[1]);
    num_producers = (uint32_t)atoi(argv[2]);
    num_consumers = (uint32_t)atoi(argv[3]);
    num_threads = num_producers + num_consumers;

    // Args array for each thread
    Args arg_array[num_threads];

    // Create the shared queue
    Queue_Create(&q, buff_size);
    Queue_Print(&q);

    // Create thread
    pthread_t threads[num_threads];
    for(id = 0; id < num_threads; id++)
    {
        // Tread ID and pointer to the Queue object
        arg_array[id].tid = id;
        arg_array[id].q = &q;

        if(id < num_producers)
        {
            // Function to call
            thread_func = producer;

            // Number of values to produce
            arg_array[id].num_values = buff_size * 2;

            printf("MAIN: P%d created (%d)\n", id, arg_array[id].num_values);
        }
        else
        {
            // Function to call
            thread_func = consumer;

            // Number of values to consume
            uint32_t total_items = num_producers * buff_size * 2;
            arg_array[id].num_values = total_items / num_consumers;

            printf("MAIN: C%d created (%d)\n", id, arg_array[id].num_values);
        }

        // create the thread
        if(pthread_create(&threads[id], NULL, thread_func, &arg_array[id]))
        {
            printf("MAIN: Error creating thread\n");
            return 1;
        }
    }

    // Wait for all the threads to finish
    for(id = 0; id < num_threads; id++)
    {
        pthread_join(threads[id], NULL);
        printf("MAIN: Thread %d DONE\n", id);
    }

    printf("MAIN: DONE\n");
    return 0;
}
