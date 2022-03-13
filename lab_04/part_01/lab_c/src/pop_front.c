#include "pop_front.h"

void *pop_front(node_t **head)
{
    void *data = NULL;
    node_t *new_head = NULL;
    if ((head) && (*head))
    {
        data = (*head)->data;
        new_head = (*head)->next;
        (*head)->next = NULL;
        free(*head);
        *head = new_head;
    }
    return data;
}
