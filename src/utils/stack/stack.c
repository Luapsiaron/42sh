#include "stack.h"

#include <stdlib.h>

// Insert an element at the top of the stack
struct stack *stack_push(struct stack *s, int e)
{
    struct stack *new_stack = malloc(sizeof(struct stack));
    if (!new_stack)
    {
        return NULL;
    }
    new_stack->data = e;
    new_stack->next = s;

    return new_stack;
}

// Remove the top most element of the stack
struct stack *stack_pop(struct stack *s)
{
    if (s == NULL)
    {
        return NULL;
    }
    struct stack *tmp = s;
    s = s->next;
    free(tmp);
    return s;
}

// Return the topmost element of the stack
int stack_peek(struct stack *s)
{
    return s->data;
}
