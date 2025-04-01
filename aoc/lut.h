#ifndef __AOC_LUT_H__
#define __AOC_LUT_H__
#include <stddef.h>

typedef unsigned long (*aoc_hashfn)(void *data, size_t data_size);

struct aoc_lut_node;
struct aoc_lut;

struct aoc_lut *aoc_new_lut(size_t shift, aoc_hashfn hashfn);
void aoc_free_lut(struct aoc_lut *lut);

struct aoc_lut_node *aoc_lut_add(struct aoc_lut *lut, unsigned long key);
void aoc_lut_remove(struct aoc_lut_node *node);

struct aoc_lut_node *aoc_lut_lookup(struct aoc_lut *lut, unsigned long key);

void *aoc_lut_container(struct aoc_lut *lut, size_t offset);

#endif /* __AOC_LUT_H__ */
