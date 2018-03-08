//
// Created by mlamonta on 3/4/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "prod_cons_MT.h"


void* producer(void* arg)
{
    Queue* buff = ((Args*)arg)->q;
    uint32_t tid = ((Args*)arg)->tid;
    uint32_t num = ((Args*)arg)->num_values;
    uint32_t new_value;
    uint32_t ii;

    printf("P%d: running\n", tid);

    // Produce num values
    for(ii = 0; ii < num; ii++)
    {
        // Generate random value
        new_value = (uint32_t)rand() % 10;

        // Put it in the queue
        printf("P%d: produced %d\n", tid, new_value);
        Queue_Enqueue(buff, new_value);
    }

    printf("P%d: DONE\n", tid);
}

void* consumer(void* arg)
{
    uint32_t val;
    Queue* buff = ((Args*)arg)->q;
    uint32_t tid = ((Args*)arg)->tid;
    uint32_t num = ((Args*)arg)->num_values;
    uint32_t ii;

    printf("C%d: consuming %d values\n", tid, num);

    // Consume num values
    for(ii = 0; ii < num; ii++)
    {
        printf("C%d: dequeue\n", tid);
        Queue_Dequeue(buff, &val);
        printf("C%d: got %d\n", tid, val);
    }

    printf("C%d: DONE\n", tid);
}


void Queue_Create(Queue* buff, uint32_t buff_size)
{
    uint32_t ii;

    // Initialize all data to zero
    for(ii = 0; ii < buff_size; ii++)
    {
        buff->data[ii] = 0;
    }

    pthread_cond_init(&buff->cv, NULL);
    pthread_mutex_init(&buff->mutex, NULL);

    buff->size = buff_size;
    buff->read = 0;
    buff->write = 0;
    buff->numel = 0;
}

void Queue_Print(Queue* buff)
{
    printf("QUEUE: r:%d w:%d s:%d n:%d \n", buff->read, buff->write, buff->size, buff->numel);
}

bool Queue_Full(Queue* buff)
{
    return buff->numel >= buff->size;
}

bool Queue_Empty(Queue* buff)
{
    return buff->numel == 0;
}

void Queue_Enqueue(Queue* buff, uint32_t val)
{
    // Wait until not full
    pthread_mutex_lock(&buff->mutex);
    while(!Queue_Full(buff))
        pthread_cond_wait(&buff->cv, &buff->mutex);

    // Add value
    buff->data[buff->write] = val;
    buff->write = (buff->write + 1) % buff->size;
    buff->numel++;

    // Print status
    Queue_Print(buff);

    // Unlock
    pthread_cond_signal(&buff->cv);
    pthread_mutex_unlock(&buff->mutex);
}

void Queue_Dequeue(Queue* buff, uint32_t* val)
{
    // Wait until not empty
    pthread_mutex_lock(&buff->mutex);
    while(!Queue_Empty(buff))
        pthread_cond_wait(&buff->cv, &buff->mutex);

    // Get value at read address
    *val = buff->data[buff->read];

    // Increment read address
    buff->read = (buff->read + 1) % buff->size;
    buff->numel--;

    // Print status
    Queue_Print(buff);

    // Unlock
    pthread_cond_signal(&buff->cv);
    pthread_mutex_unlock(&buff->mutex);
}
