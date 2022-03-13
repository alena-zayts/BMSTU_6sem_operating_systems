#ifndef TASK_H
#define TASK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main_header.h"
#include "my_getdelim.h"

typedef struct node node_t;

struct node
{
    void *data;
    node_t *next;
};

typedef struct my_data data_t;

struct my_data
{
    char *name;
    int diff;
};

int my_comparator(const void *x, const void *y);

node_t *task_create_node(char *name, int diff);
node_t *task_read(FILE *f);
void task_print(FILE *f, node_t *node);
node_t *task_free(node_t *node);


#endif // TASK_H
