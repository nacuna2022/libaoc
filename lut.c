#include <stdlib.h>
#include <assert.h>

#include <aoc/lut.h>

struct aoc_lut_node {
	unsigned long key;
	struct aoc_lut *lut;
	struct aoc_lut_node *link;
};

struct aoc_lut {
	size_t shift;
	size_t size;
	aoc_hashfn hashfn;
	struct aoc_lut_node *table[];
};

static unsigned long fnv1a(void *data, size_t data_size)
{
        size_t i;
        unsigned char *pdata = (unsigned char *)data;
        static const unsigned long fnv1a_offset = 0xcbf29ce484222325;
        static const unsigned long fnv1a_prime = 0x100000001b3;
        unsigned long h;
        h = fnv1a_offset;
        for (i = 0; i < data_size; i++) {
                h = h ^ pdata[i];
                h = h * fnv1a_prime;
        }
        return h;
}

static unsigned long compute_hash(struct aoc_lut *lut, void *data,
	size_t data_size)
{
	if (lut->hashfn)
		return lut->hashfn(data, data_size);
	return fnv1a(data, data_size);
}

static struct aoc_lut *init_lut(struct aoc_lut *lut)
{
	size_t i;
	for (i = 0; i < lut->size; i += 1) {
		lut->table[i] = NULL;
	}
	return lut;
}

struct aoc_lut *aoc_new_lut(size_t shift, aoc_hashfn hashfn)
{
	size_t size;
	struct aoc_lut *lut;
	assert(shift > 0);
	
	size = sizeof(struct aoc_lut) + (sizeof(struct aoc_lut_node *) * (1 << shift));
	if ((lut = malloc(size)) == NULL)
		return NULL;

	lut->shift = shift;
	lut->size = 1 << shift;
	lut->hashfn = hashfn;
	return init_lut(lut);
}

static void free_lut_chain(struct aoc_lut_node *node)
{
	while(node) {
		struct aoc_lut_node *tmp = node->link;
		free(node);
		node = tmp;
	}
	return;
}

void aoc_free_lut(struct aoc_lut *lut)
{
	size_t i;
	assert(lut != NULL);
	for (i = 0; i < lut->size; i += 1) {
		if (lut->table[i] == NULL)
			continue;
		free_lut_chain(lut->table[i]);
		lut->table[i] = NULL;
	}
	free(lut);
	return;
}

struct aoc_lut_node *new_lut_node(struct aoc_lut_node **nodepp,
	struct aoc_lut *lut, unsigned long key)
{
	struct aoc_lut_node *node;
	assert(nodepp != NULL);
	if ((node = malloc(sizeof * node)) != NULL) {
		node->key = key;
		node->lut = lut;
		node->link = NULL;
		*nodepp = node;
	}
	return node;
}

struct aoc_lut_node *aoc_lut_add(struct aoc_lut *lut, unsigned long key)
{
	size_t idx;
	struct aoc_lut_node **nodepp;
	struct aoc_lut_node *node;
	assert(lut != NULL);
	idx = compute_hash(lut, &key, sizeof(key));
	idx = idx & (lut->size - 1);
	nodepp = &lut->table[idx];
	for (;;) {
		node = *nodepp;
		if (node == NULL)
			break;

		if (node->key == key)
			return node;

		nodepp = &(node->link);
	}
	return new_lut_node(nodepp, lut, key);
}

void aoc_lut_remove(struct aoc_lut_node *node)
{
	size_t idx;
	struct aoc_lut *lut;
	struct aoc_lut_node **nodepp;
	struct aoc_lut_node *tmp;
	assert(node != NULL);
	assert(node->lut != NULL);
	lut = node->lut;
	idx = compute_hash(lut, &node->key, sizeof(node->key));
	idx = idx & (lut->size - 1);
	nodepp = &lut->table[idx];
	for (;;) {
		tmp = *nodepp;
		if (tmp == NULL)
			break;

		if (tmp == node) {
			*nodepp = tmp->link;
			free(tmp);
			break;
		}
		nodepp = &(tmp->link);
	}
	return;
}

struct aoc_lut_node *aoc_lut_lookup(struct aoc_lut *lut, unsigned long key)
{
	size_t idx;
	struct aoc_lut_node **nodepp;
	struct aoc_lut_node *tmp;
	assert(lut != NULL);
	idx = compute_hash(lut, &key, sizeof key);
	idx = idx & (lut->size - 1);
	nodepp = &lut->table[idx];
	for (;;) {
		tmp = *nodepp;
		if (tmp == NULL)
			break;

		if (tmp->key == key) {
			return tmp;
		}
		nodepp = &(tmp->link);
	}
	return NULL;
}

void *aoc_lut_container(struct aoc_lut *lut, size_t offset)
{
	char *ptr = (char *)lut;
	return (void*)(ptr - offset);
}

