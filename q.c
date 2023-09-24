#include "q.h"
#include <string.h>

Queue_T* create_queue(void)
{
    Queue_T* q = malloc(sizeof(Queue_T));
    q->size = 0u;
    q->front = 0;
    q->rear = -1;

    return q;
}

void delete_queue(Queue_T* q)
{
    free(q);
}

void enqueue(Queue_T* q, void* elem)
{
    if(q->size == QUEUE_CAP)
    {
        assert(false);
    }
    else
    {
        ++q->size;
        q->rear += 1;
        if(q->rear == QUEUE_CAP)
        {
            q->rear = 0;
        }

        q->elems[q->rear] = elem;
    }
}

void* dequeue(Queue_T* q)
{
    if(q->size == 0u)
    {
        return NULL;
    }

    void* item = q->elems[q->front];
    --q->size;
    ++q->front;

    if(q->front == QUEUE_CAP)
    {
        q->front = 0;
    }
    return item;
}

void* front(Queue_T* q)
{
    return q->elems[q->front];
}

void* peek(Queue_T* q, U8 pos)
{
    if(pos < q->size)
    {
        return q->elems[pos];
    }
    return NULL;
}
