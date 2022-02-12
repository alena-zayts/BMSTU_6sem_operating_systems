#include "stack.h"

void init(struct stack *st)
{
    st->size = 0;
}

int is_empty(struct stack *st)
{
    return (st->size == 0) ? 1 : 0;
}

int push(struct stack *st, struct stackItem *item)
{
    if (st->size == MAX_STACK_SIZE)
    {
        printf("Stack overflow");
        return -1;
    }

    strcpy(st->items[st->size].fileName, item->fileName);
    st->items[st->size].depth = item->depth;
    st->size++;
	
	return 0;
}

struct stackItem pop(struct stack *st)
{
	struct stackItem item = {.fileName = "", .depth = 0};
	
    if ((st->size) == 0)
	{
		printf("Stack is empty\n");
	}
	else
	{
		st->size--;
		strcpy(item.fileName, st->items[st->size].fileName);
		item.depth = st->items[st->size].depth;
	}
	
	return item;
}
