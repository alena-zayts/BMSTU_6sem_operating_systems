#include "insert.h"

void insert(node_t **head, node_t *elem, node_t *before)
{
    //source data is ok
    if (*head && elem && (elem->next == NULL))
    {
        node_t *cur = *head;

        //insert in begining
        if (cur == before)
        {
            elem->next = cur;
            *head = elem;
        }
        //insert in middle/end
        else
        {
            while (cur && (cur->next != before))
                cur = cur->next;
            //found "before"
            if (cur)
            {
                cur->next = elem;
                elem->next = before;
            }
            //else - error
        }
    }
    //else - error
}
