#ifndef FIFO_H
#define FIFO_H

typedef struct fifo
{
    struct fifo *next;
    struct fifo *previous;
    int data;
} fifo_t;

fifo_t *fifo_init();
void fifo_push(fifo_t *file, struct fifo f);

#endif /* ! FIFO_H */