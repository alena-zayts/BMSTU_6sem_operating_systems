#include "task.h"

int my_comparator(const void *x, const void *y)
{
    data_t *data_x = (data_t *) x;
    data_t *data_y = (data_t *) y;

    if (data_x->diff > data_y->diff)
        return 1;
    if (data_x->diff < data_y->diff)
        return -1;
    if (strcmp(data_x->name, data_y->name) > 0)
        return 1;
    if (strcmp(data_x->name, data_y->name) < 0)
        return -1;
    return 0;
}

node_t *task_create_node(char *name, int diff)
{
    char *name_tmp = NULL;
    node_t *node = NULL;
    data_t *data_tmp = NULL;

    node = malloc(sizeof(node_t));
    if (node)
    {
        data_tmp = malloc(sizeof(data_t));
        if (data_tmp)
        {
            name_tmp = calloc(strlen(name) + 1, sizeof(char));
            if (name_tmp)
            {
                strncpy(name_tmp, name, strlen(name) + 1);
                data_tmp->name = name_tmp;
                data_tmp->diff = diff;
                node->data = data_tmp;
                node->next = NULL;
            }
            else
            {
                free(data_tmp);
                free(node);
                node = NULL;
            }
        }
        else
        {
            free(node);
            node = NULL;
        }
    }
    else
        node = NULL;
    return node;
}

node_t *task_read(FILE *f)
{
    node_t *node = NULL;
    int rc = OK;

    char *name = NULL;
    int diff = 0;

    size_t len = 0;

    if ((rc = my_getdelim(&name, &len, '\r', f)) == OK)
    {
        if ((fscanf(f, "%d", &diff) == 1) && ((fgetc(f) == '\r') || (feof(f))))
        {
            node = task_create_node(name, diff);
        }
        free(name);
    }
    return node;
}

void task_print(FILE *f, node_t *node)
{
    fprintf(f, "%s\n", ((data_t *) (node->data))->name);
    fprintf(f, "%d\n", ((data_t *) (node->data))->diff);
}

node_t *task_free(node_t *node)
{
    node_t *next = NULL;
    if (node)
    {
        next = node->next;
        node->next = NULL;

        free(((data_t *) (node->data))->name);
        free(node->data);
        free(node);
    }
    return next;
}

