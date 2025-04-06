#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <aoc/stack.h>

struct stack_data {
	struct stack_data *prev;
	char __attribute__((aligned(8))) data[];
};

struct aoc_stack {
	size_t data_size;
	struct stack_data *top;
};

static struct stack_data *new_stack_data(struct aoc_stack *stack, void *data)
{
	struct stack_data *stack_data;
	assert(stack != NULL);
	assert(data != NULL);
	if ((stack_data = malloc(sizeof(struct stack_data) + stack->data_size)) != NULL) {
		stack_data->prev = NULL;
		memcpy(stack_data->data, data, stack->data_size);
	}
	return stack_data;
}

struct aoc_stack *aoc_new_stack(size_t data_size)
{
	struct aoc_stack *stack;
	if ((stack = malloc(sizeof(struct aoc_stack))) != NULL) {
		stack->data_size = data_size;
		stack->top = NULL;
	}
	return stack;
}

/* we pop until we are not able to, then release the stack itself */
void aoc_free_stack(struct aoc_stack *stack)
{
	assert(stack != NULL);
	while(aoc_stack_pop(stack, NULL) != -1);
	free(stack);
	return;
}

int aoc_stack_push(struct aoc_stack *stack, void *data)
{
	int ret = -1;
	assert(stack != NULL);
	assert(data != NULL);
	struct stack_data *stack_data = new_stack_data(stack, data);
	if (stack_data) {
		stack_data->prev = stack->top;
		stack->top = stack_data;
		ret = 0;
	}
	return ret;
}

int aoc_stack_pop(struct aoc_stack *stack, void *data)
{
	int err = -1;
	struct stack_data *d;
	assert(stack != NULL);
	if (stack->top != NULL) {
		d = stack->top;
		stack->top = d->prev;
		if (data)
			memcpy(data, d->data, stack->data_size);
		free(d);
		err = 0;
	}
	return err; 
}

