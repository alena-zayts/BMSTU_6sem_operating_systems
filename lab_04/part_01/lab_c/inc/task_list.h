#ifndef TASK_LIST_H
#define TASK_LIST_H

#include "task.h"
#include "insert.h"

void task_list_free(node_t *head);
node_t *task_list_read(FILE *f);
void task_list_print(FILE *f, node_t *head);
void task_list_insert_sorted(node_t **head, node_t *elem, int (*comparator)(const void *, const void *));

data_t **create_deletion_list(node_t *head);
void free_deletion_list(data_t **list, node_t *head);

#endif // TASK_LIST_H
