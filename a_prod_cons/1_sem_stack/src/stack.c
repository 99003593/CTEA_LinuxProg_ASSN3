#include "stack.h"
#include <stddef.h>
#include <stdlib.h>

struct stack_s
{
    void** buff;
    int top;
    int capacity;
};

stack_t* stack_create(int capacity)
{
    stack_t *p_stk = (stack_t*)malloc(sizeof(stack_t));
    p_stk->top = -1;
    p_stk->capacity = capacity;
    p_stk->buff = (void**)malloc(sizeof(void*)*capacity);

    return p_stk;
}

bool stack_isfull(stack_t* p_stk)
{
    bool ret = true;
    if (p_stk != NULL)
        ret = (p_stk->top == p_stk->capacity);
    return ret;
}

bool stack_isempty(stack_t* p_stk)
{
    bool ret = true;
    if (p_stk != NULL)
        ret = (p_stk->top == -1);
    return ret;
}

void stack_push(stack_t* p_stk, void* p_item)
{
    if(!stack_isfull(p_stk))
    {
        p_stk->top++;
        p_stk->buff[p_stk->top] = p_item;
    }
}

int stack_size(stack_t* p_stk)
{
    return p_stk->capacity;
}

void* stack_peek(stack_t* p_stk, int pos)
{
    void* p_item = NULL;
    if (!stack_isempty(p_stk) && pos <= p_stk->top)
    {
        p_item = p_stk->buff[pos];
    }
    return p_item;
}

void* stack_pop(stack_t* p_stk)
{
    void* p_item = NULL;
    if (!stack_isempty(p_stk))
    {
        p_item = p_stk->buff[p_stk->top];
        p_stk->buff[p_stk->top] = NULL;
        p_stk->top--;
    }
    return p_item;
}

void stack_destroy(stack_t *p_stk)
{
    void* p_item;
    while((p_item = stack_pop(p_stk)) != NULL)
    {
        free(p_item);
    }
    free(p_stk->buff);
    free(p_stk);
}
