#ifndef __AOC_QUEUE_H__
#define __AOC_QUEUE_H__
#include <stddef.h>

struct aoc_queue;

struct aoc_queue *aoc_new_queue(size_t data_size);
void aoc_free_queue(struct aoc_queue *q);

int aoc_queue(struct aoc_queue *q, void *src, size_t src_size);
int aoc_dequeue(struct aoc_queue *q, void *dst, size_t dst_size);

#endif /* __AOC_QUEUE_H__ */

