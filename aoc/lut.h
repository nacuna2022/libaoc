#ifndef __AOC_LUT_H__
#define __AOC_LUT_H__
#include <stddef.h>

typedef unsigned long (*aoc_hashfn)(void *data, size_t data_size);

struct aoc_lut;
struct aoc_lut_node {
	unsigned long key;
	struct aoc_lut *lut;
	struct aoc_lut_node *link;
	char __attribute__((aligned(8))) data[];
};

struct aoc_lut *aoc_new_lut(size_t shift, size_t data_size, aoc_hashfn hashfn);
void aoc_free_lut(struct aoc_lut *lut);

struct aoc_lut_node *aoc_lut_add(struct aoc_lut *lut, unsigned long key);
void aoc_lut_remove(struct aoc_lut_node *node);

struct aoc_lut_node *aoc_lut_lookup(struct aoc_lut *lut, unsigned long key);

/* sometimes we want to traverse the lut */
int aoc_lut_node_idx(struct aoc_lut *lut, struct aoc_lut_node **nodepp, size_t i);

void *aoc_lut_node_data(struct aoc_lut_node *node);

#endif /* __AOC_LUT_H__ */
