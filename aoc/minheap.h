#ifndef __AOC_MINHEAP_H__
#define __AOC_MINHEAP_H__
#include <stddef.h>

struct aoc_minheap;

struct aoc_minheap *aoc_new_minheap(size_t payload_size);
void aoc_free_minheap(struct aoc_minheap *minheap);

int aoc_minheap_insert(struct aoc_minheap **minheap, int key, void *payload, 
	size_t payload_size);
int aoc_minheap_get(struct aoc_minheap *minheap, int *key, void *payload,
	size_t payload_size);
int aoc_minheap_peek(struct aoc_minheap *minheap, int *key, void *payload,
	size_t payload_size);

#endif /* __AOC_MINHEAP_H__ */

