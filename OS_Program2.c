#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>


#define MAX_QUEUE_SIZE 1000


typedef struct
{
    uint32_t size;
    uint32_t data[MAX_QUEUE_SIZE];
    uint32_t read;
    uint32_t write;
    pthread_mutex_t mutex;

} Queue;


void Queue_Create(Queue* buff, uint32_t buff_size)
{
    uint32_t ii;
    
    for(ii = 0; ii < buff_size; ii++)
    {
        buff->data[ii] = 0;
    }
    
    buff->size = buff_size;
    buff->read = 0;
    buff->write = 0;
}

void Queue_Enqueue(Queue* buff, uint32_t val)
{
    /* Wait for room */
    while(buff->write != buff->read);
    
    pthread_mutex_lock(&buff->mutex);
    buff->data[buff->write] = val;
    buff->write = (buff->write + 1) % buff->size;
    pthread_mutex_unlock(&buff->mutex);
}

void Queue_Dequeue(Queue* buff, uint32_t* val)
{
    /* Wait for data */
    while(buff->read == buff->write);
    
    pthread_mutex_lock(&buff->mutex);
    *val = buff->data[buff->read];
    buff->read = (buff->read + 1) % buff->size;
    pthread_mutex_unlock(&buff->mutex);
}


void* producer(void* arg)
{
    Queue* buff = (Queue*)arg;
    uint32_t new_value = rand() % 10;
    
    printf("producer %d\n", new_value);

    Queue_Enqueue(buff, new_value);
}

void* consumer(void* arg)
{
    uint32_t val;
    Queue* buff = (Queue*)arg;

    Queue_Dequeue(buff, &val);
    
    printf("consumer %d\n", val);
}


int main(int argc, void** argv)
{
    uint32_t id;
    uint32_t buff_size;
    uint32_t num_producers;
    uint32_t num_consumers;
    uint32_t num_threads;
    void* thread_func;
    Queue q;
    
    printf("MAIN\n");
    
    /* Seed random */
    srand(time(0));
    
    /* Check number of arguments */
    if(argc != 4)
    {
        printf("USAGE: %s <buffer size> <#producers> <#consumers>\n");
        return 1;
    }
    
    /* Parse the arguments */
    buff_size = atoi(argv[1]);
    num_producers = atoi(argv[2]);
    num_consumers = atoi(argv[3]);
    num_threads = num_producers + num_consumers;
    
    /* Create the shared queue */
    Queue_Create(&q, buff_size);
        
    /* Create threads */
    pthread_t threads[num_threads];
    for(id = 0; id < num_threads; id++)
    {
        if(id < num_producers)
        {
            thread_func = producer;
        }
        else
        {
            thread_func = consumer;
        }
        
        /* create the thread */
        if(pthread_create(&threads[id], NULL, thread_func, &q))
        {
            printf("Error creating thread\n");
            return 1;
        }
    }
    
    /* Waif for all the threads to finish */
    for(id = 0; id < num_threads; id++)
    {
        pthread_join(threads[id], NULL);
        printf("Thread %d DONE\n", id);
    }
    
    printf("DONE\n");
    return 0;
}
