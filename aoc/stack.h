#ifndef __AOC_STACK_H__
#define __AOC_STACK_H__

struct aoc_stack;

struct aoc_stack *aoc_new_stack(size_t data_size);
void aoc_free_stack(struct aoc_stack *stack);

int aoc_push_stack(struct aoc_stack *stack, void *data);
int aoc_pop_stack(struct aoc_stack *stack, void *data);

#endif /* __AOC_STACK_H__ */

