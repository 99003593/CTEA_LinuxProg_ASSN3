#include "cirque.h"
#include <stddef.h>
#include <stdlib.h>

struct cirque_s
{
    void **buff;
    int front;
    int rear;
    int capacity;
};

cirque_t *cirque_create(int capacity)
{
    cirque_t *p_crq = (cirque_t *)malloc(sizeof(cirque_t));
    p_crq->front = 0;
    p_crq->rear = 0;
    p_crq->capacity = capacity;
    p_crq->buff = (void **)malloc(sizeof(void *) * capacity);

    return p_crq;
}

bool cirque_isfull(cirque_t *p_crq)
{
    bool ret = true;
    if (p_crq != NULL)
        ret = ((p_crq->front + 1) % p_crq->capacity == p_crq->rear);
    return ret;
}

bool cirque_isempty(cirque_t *p_crq)
{
    bool ret = true;
    if (p_crq != NULL)
        ret = (p_crq->front == p_crq->rear && p_crq->buff[p_crq->rear] == NULL);
    return ret;
}

void cirque_enqueue(cirque_t *p_crq, void *p_item)
{
    if (!cirque_isfull(p_crq))
    {
        p_crq->buff[p_crq->front] = p_item;
        p_crq->front = (p_crq->front + 1) % p_crq->capacity;
    }
}

int cirque_size(cirque_t *p_crq)
{
    return p_crq->capacity + 1;
}

void *cirque_peek(cirque_t *p_crq, int pos)
{
    void *p_item = NULL;
    if (!cirque_isempty(p_crq))
    {
        p_item = p_crq->buff[pos];
    }
    return p_item;
}

void *cirque_dequeue(cirque_t *p_crq)
{
    void *p_item = NULL;
    if (!cirque_isempty(p_crq))
    {
        p_item = p_crq->buff[p_crq->rear];
        p_crq->buff[p_crq->rear] = NULL;
        p_crq->rear = (p_crq->rear + 1) % p_crq->capacity;
    }
    return p_item;
}

void cirque_destroy(cirque_t *p_crq)
{
    void *p_item;
    while ((p_item = cirque_dequeue(p_crq)) != NULL)
    {
        free(p_item);
    }
    free(p_crq->buff);
    free(p_crq);
}
