//
// Created by mlamonta on 3/4/18.
//

#ifndef OS_PROGRAM2_PROD_CONS_MT_H
#define OS_PROGRAM2_PROD_CONS_MT_H

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>


#define MAX_QUEUE_SIZE 1000

// Queue object
typedef struct
{
    uint32_t size;
    uint32_t data[MAX_QUEUE_SIZE];
    uint32_t read;
    uint32_t write;
    uint32_t numel;
    pthread_cond_t cv;
    pthread_mutex_t mutex;

} Queue;

// Argument object to the thread functions
typedef struct
{
    uint32_t tid;
    Queue* q;
    uint32_t num_values;
} Args;


void* producer(void* arg);
void* consumer(void* arg);

void Queue_Create(Queue* buff, uint32_t buff_size);
void Queue_Print(Queue* buff);
bool Queue_Full(Queue* buff);
bool Queue_Empty(Queue* buff);
void Queue_Enqueue(Queue* buff, uint32_t val);
void Queue_Dequeue(Queue* buff, uint32_t* val);


#endif //OS_PROGRAM2_PROD_CONS_MT_H
