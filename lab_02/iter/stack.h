#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <string.h>

#define MAX_STACK_SIZE 1000000
#define MAX_FILENAME_LEN 100

struct stackItem
{
    char fileName[MAX_FILENAME_LEN];
    int depth;
};

struct stack
{
    struct stackItem items[MAX_STACK_SIZE];
    int size;
};

void init(struct stack *st);
int is_empty(struct stack *st);
int push(struct stack *st, struct stackItem *item);
struct stackItem pop(struct stack *st);

#endif
