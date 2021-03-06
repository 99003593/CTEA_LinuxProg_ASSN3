#ifndef __CIRQUE_H_
#define __CIRQUE_H_

#include <stdbool.h>

typedef struct cirque_s cirque_t;

/**
 * Ownership Contract:
 *      Any element enqueued to cirque becomes owned by the cirque.
 *      Any element dequeued from the cirque becomes owned by the user code.
 *      Pass heap memory pointers as elements of cirque.
 */

cirque_t *cirque_create(int capacity);
bool cirque_isfull(cirque_t *p_crq);
bool cirque_isempty(cirque_t *p_crq);
void cirque_enqueue(cirque_t *p_crq, void *p_item);
int cirque_size(cirque_t *p_crq);
void *cirque_peek(cirque_t *p_crq, int pos);
void *cirque_dequeue(cirque_t *p_crq);
void cirque_destroy(cirque_t *p_crq);

#endif // __CIRQUE_H_
