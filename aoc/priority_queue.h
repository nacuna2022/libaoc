#ifndef __AOC_PRIORITY_QUEUE_H__
#define __AOC_PRIORITY_QUEUE_H__
#include <stddef.h>

struct aoc_priority_queue;

struct aoc_priority_queue *aoc_new_priority_queue(size_t payload_size);
void aoc_free_priority_queue(struct aoc_priority_queue *prio_queue);

int aoc_priority_queue_insert(struct aoc_priority_queue **prio_queue, int key, void *payload, 
	size_t payload_size);
int aoc_priority_queue_get_min(struct aoc_priority_queue *prio_queue, int *key, void *payload,
	size_t payload_size);
int aoc_priority_queue_peek(struct aoc_priority_queue *prio_queue, int *key, void *payload,
	size_t payload_size);

#endif /* __AOC_PRIORITY_QUEUE_H__ */

