#include "sort.h"

void front_back_split(node_t *head, node_t **back)
{
    node_t *cur = head;
    int amount = 0;
    //count amount
    while (cur)
    {
        amount++;
        cur = cur->next;
    }
    //mistake
    if (amount < 2)
    {
        *back = NULL;
    }
    else
    {
        cur = head;
        for (int i = 0; i < (amount - 1) / 2; i++, cur = cur->next);
        *back = cur->next;
        cur->next = NULL;
    }
}



node_t *sorted_merge(node_t **heada, node_t **headb, int (*comparator)(const void *, const void *))
{
    node_t *head_final = NULL;
    node_t *cur = NULL;

    //while at least one is unempty
    while (*heada || *headb)
    {
        //if both are unempty
        if (*heada && *headb)
        {
            //take from a
            if ((comparator((*heada)->data, (*headb)->data) <= 0))
            {
                if (cur == NULL)
                {
                    cur = *heada;
                    head_final = cur;
                }
                else
                {
                    cur->next = *heada;
                    cur = cur->next;
                }
                *heada = (*heada)->next;
                cur->next = NULL;
            }
            //take from b
            else
            {
                if (cur == NULL)
                {
                    cur = *headb;
                    head_final = cur;
                }
                else
                {
                    cur->next = *headb;
                    cur = cur->next;
                }
                *headb = (*headb)->next;
                cur->next = NULL;
            }
        }
        //if only a unempty
        //take from a
        else if (*heada && !(*headb))
        {
            if (cur == NULL)
            {
                cur = *heada;
                head_final = cur;
            }
            else
            {
                cur->next = *heada;
                cur = cur->next;
            }
            *heada = (*heada)->next;
            cur->next = NULL;
        }
        //if only b unempty
        //take from b
        else
        {
            if (cur == NULL)
            {
                cur = *headb;
                head_final = cur;
            }
            else
            {
                cur->next = *headb;
                cur = cur->next;
            }
            *headb = (*headb)->next;
            cur->next = NULL;
        }
    }
    return head_final;
}


node_t *sort(node_t *head, int (*comparator)(const void *, const void *))
{
    node_t *back = NULL;
    if ((head) && (head->next))
    {
        //split
        front_back_split(head, &back);

        //sort halves
        head = sort(head, comparator);
        back = sort(back, comparator);

        //join halves
        head = sorted_merge(&head, &back, comparator);
    }
    return head;
}
