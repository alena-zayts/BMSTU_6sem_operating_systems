#include "task_list.h"

void task_list_free(node_t *head)
{
    node_t *cur = head;
    while (cur)
    {
        cur = task_free(cur);
    }
}

node_t *task_list_read(FILE *f)
{
    int rc = OK;
    node_t *node = NULL;
    node_t *head = NULL;
    while ((!feof(f)) && (rc == OK))
    {
        node = task_read(f);
        //read successfully
        if (node)
        {
            //but diff is invalid
            if (((data_t *) node->data)->diff < MINDIFF)
            {
                task_free(node);
                rc = ERRVALUE;
                task_list_free(head);
                head = NULL;
            }
            //diff is valid
            else
            {
                //first element in list
                if (!head)
                    head = node;
                //not first - add in the end
                else
                    insert(&head, node, NULL);
            }
        }
        //read error
        else if ((fgetc(f) != '\n') && (!feof(f)))
        {
            rc = ERRVALUE;
            task_list_free(head);
            head = NULL;
        }
    }
    return head;
}

void task_list_print(FILE *f, node_t *head)
{
    if (head)
    {
        fprintf(f, "Choose one of this:\n");
        node_t *cur = head;
        while (cur)
        {
            task_print(f, cur);
            cur = cur->next;
        }
    }
    else
        fprintf(f, "Nothing left\n");
}

void task_list_insert_sorted(node_t **head, node_t *elem, int (*comparator)(const void *, const void *))
{
    node_t *cur = *head;
    while ((cur) && (comparator(cur->data, elem->data) <= 0))
        cur = cur->next;
    insert(head, elem, cur);
}

data_t **create_deletion_list(node_t *head)
{
    //count total amount of elems
    int amount = 0;
    node_t *cur = head;

    while (cur)
    {
        amount++;
        cur = cur->next;
    }
    //create list of nodes to delete
    data_t **to_delete = NULL;
    to_delete = calloc((amount + 1), sizeof(data_t *));
    if (to_delete)
    {
        cur = head;
        if (head)
            for (int i = 0; i < amount; i++, cur = cur->next)
            {
                to_delete[i] = cur->data;
            }
        to_delete[amount] = NULL;
    }
    else
    {
        to_delete = NULL;
    }
    return to_delete;
}

void free_deletion_list(data_t **list, node_t *head)
{
    int flag;
    for (int i = 0; list[i]; i++)
    {
        flag = 0;
        for (node_t *cur = head; cur; cur = cur->next)
            if (list[i] == cur->data)
                flag = 1;
        if (flag == 0)
        {
            free(list[i]->name);
            free(list[i]);
        }
    }
    free(list);
}
