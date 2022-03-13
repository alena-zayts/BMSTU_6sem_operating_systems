#ifndef SORT_H
#define SORT_H

#include "task.h"

void front_back_split(node_t *head, node_t **back);
node_t *sorted_merge(node_t **heada, node_t **headb, int (*comparator)(const void *, const void *));
node_t *sort(node_t *head, int (*comparator)(const void *, const void *));

#endif // SORT_H
