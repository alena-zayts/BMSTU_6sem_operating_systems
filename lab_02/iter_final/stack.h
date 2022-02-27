#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct stack_node_t
{
    char *filename;
    int depth;
	
	struct stack_node_t *next;
};

struct stack_node_t* stack_node_create(char *filename, int depth);
struct stack_node_t* push(struct stack_node_t *top, struct stack_node_t* node);
struct stack_node_t* pop(struct stack_node_t **top);
void stack_node_free(struct stack_node_t *top);
void stack_free(struct stack_node_t *top);

#endif
