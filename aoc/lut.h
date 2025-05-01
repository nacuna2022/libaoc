#ifndef __AOC_LUT_H__
#define __AOC_LUT_H__
#include <stddef.h>

struct aoc_lut;

struct aoc_lut *aoc_new_lut(size_t shift, size_t keylen, size_t datalen);
void aoc_free_lut(struct aoc_lut *lut);

int aoc_lut_add(struct aoc_lut *lut, void *key, size_t keylen,
	void *data, size_t datalen);
int aoc_lut_lookup(struct aoc_lut *lut, void *key, size_t keylen,
	void *data, size_t datalen);
int aoc_lut_remove(struct aoc_lut *lut, void *key, size_t keylen);

/* sometimes (especially for debugging purposes), we want to know how many 
 * items still left in the lut */
int aoc_lut_item_count(struct aoc_lut *lut);

/* sometimes we want to just go through each item in the lut to do something */
typedef void aoc_lut_callback(const void *key, const void *data, void *param);
int aoc_lut_foreach(struct aoc_lut *lut, aoc_lut_callback cb, void *cb_param);

#endif /* __AOC_LUT_H__ */
