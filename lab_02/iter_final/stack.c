#include "stack.h"

struct stack_node_t* stack_node_create(char *filename, int depth)
{
	struct stack_node_t  *node = malloc(sizeof(struct stack_node_t));
	
	if (node)
	{
		node->filename = malloc((strlen(filename) + 1) * sizeof(char));
		if (node->filename)
		{
			strcpy(node->filename, filename);
			node->depth = depth;
			node->next =  NULL;
		}
		else
		{
			free(node);
			node = NULL;
		}
	}
	
	return node;
}

struct stack_node_t* push(struct stack_node_t *top, struct stack_node_t* node)
{
	node->next = top;
	return node;
}

struct stack_node_t* pop(struct stack_node_t **top)
{
	struct stack_node_t *node = *top;
	*top = (*top)->next;
	return node;
}

void stack_node_free(struct stack_node_t *node)
{	
	free(node->filename);
	free(node);
}

void stack_free(struct stack_node_t *top)
{
	struct stack_node_t *next = NULL;
	
	for (; top; top = next)
	{
		next = top->next;
		stack_node_free(top);
	}
}
