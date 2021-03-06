#ifndef __STACK_H_
#define __STACK_H_

#include <stdbool.h>

typedef struct stack_s stack_t;

/**
 * Ownership Contract:
 *      Any element pushed to stack becomes owned by the stack.
 *      Any element poped from the stack becomes owned by the user code.
 *      Pass heap memory pointers as elements of stack.
 */

stack_t *stack_create(int capacity);
bool stack_isfull(stack_t *p_stk);
bool stack_isempty(stack_t *p_stk);
void stack_push(stack_t *p_stk, void *p_item);
int stack_size(stack_t *p_stk);
void *stack_peek(stack_t *p_stk, int pos);
void *stack_pop(stack_t *p_stk);
void stack_destroy(stack_t *p_stk);

#endif // __STACK_H_
