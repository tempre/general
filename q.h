#ifndef Q_H_
#define Q_H_

#include "common.h"

#define QUEUE_CAP 100u

typedef struct Queue_T
{
    U8 size;
    I16 front;
    I16 rear;
    void* elems[QUEUE_CAP];
} Queue_T;

Queue_T* create_queue(void);
void delete_queue(Queue_T* q);
void enqueue(Queue_T* q, void* elem);
void* dequeue(Queue_T* q);
void* peek(Queue_T* q, U8 pos);
void* front(Queue_T* q);


#endif // Q_H_
