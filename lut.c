#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include <aoc/lut.h>

struct lut_node {
	void *key;
	struct aoc_lut *lut;
	struct lut_node *link;
	char __attribute__((aligned(8))) data[];
};

struct aoc_lut {
	size_t shift;
	size_t keylen;
	size_t datalen;
	struct lut_node *hash_table[];
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

struct aoc_lut *aoc_new_lut(size_t shift, size_t keylen, size_t datalen)
{
	size_t size;
	struct aoc_lut *lut;
	if ((shift == 0) || (keylen == 0)) {
		return NULL;
	}

	size = (sizeof(struct lut_node *) * (1 << shift));
	if ((lut = malloc(sizeof(struct aoc_lut) + size)) != NULL) {
		int i;
		lut->shift = shift;
		lut->keylen = keylen;
		lut->datalen = datalen;
		for (i = 0; i < (1 <<shift); i += 1) {
			lut->hash_table[i] = NULL;
		}
	}
	return lut;
}

static void free_lut_node(struct lut_node *node)
{
	assert(node != NULL);
	free(node->key);
	free(node);
	return;
}

static void free_hash_chain(struct lut_node *node)
{
	struct lut_node *curnode;
	assert(node != NULL);
	for (curnode = node; curnode != NULL; ) {
		struct lut_node *tmp;
		tmp = curnode->link;
		free_lut_node(curnode);
		curnode = tmp;
	}
	return;
}

void aoc_free_lut(struct aoc_lut *lut)
{
	int i;
	assert(lut != NULL);
	for (i = 0; i < (1 << lut->shift); i += 1) {
		if (lut->hash_table[i] != NULL) {
			free_hash_chain(lut->hash_table[i]);
		}
	}
	free(lut);
	return;
}

static bool lutkey_equal(struct aoc_lut *lut, void *k1, void *k2)
{
	assert(lut != NULL);
	assert(k1 != NULL);
	assert(k2 != NULL);
	return memcmp(k1, k2, lut->keylen) == 0;
}

static int init_node(struct aoc_lut *lut, struct lut_node *node,
	void *key, size_t keylen, void *data, size_t datalen)
{
	char *ptr;
	int err = -1;
	assert(node != NULL);
	assert(key != NULL);
	assert(keylen > 0);
	assert(keylen == lut->keylen);
	
	/* initialize the node key information */
	if ((ptr = malloc(keylen)) != NULL) {
		memcpy(ptr, key, keylen);
		node->key = ptr;
		node->lut = lut;
		node->link = NULL;
		
		/* now do the rest */
		memcpy(node->data, data, datalen);

		err = 0;
	}
	return err;
}

static int new_lut_node(struct aoc_lut *lut, struct lut_node **nodepp,
	void *key, size_t keylen, void *data, size_t datalen)
{
	int ret = -1;
	struct lut_node *node;
	assert(nodepp != NULL);
	assert(*nodepp == NULL);
	if ((node = malloc(sizeof(struct lut_node) + datalen)) != NULL) {
		ret = init_node(lut, node, key, keylen, data, datalen);
		*nodepp = node;
		ret = 0;
	}
	return ret;
}


int aoc_lut_add(struct aoc_lut *lut, void *key, size_t keylen, 
	void *data, size_t datalen)
{
	int ret = -1;
	unsigned long idx;
	struct lut_node **nodepp;
	assert(lut != NULL);
	assert(key != NULL);
	assert(keylen == lut->keylen);
	if (datalen > 0) {
		assert(data != NULL);
		assert(datalen == lut->datalen);
	}

	/* generate a hash table index from our key and normalize it to 
	 * our hash table size */
	idx = fnv1a(key, keylen);
	idx = idx & ((1 << lut->shift) - 1);

	nodepp = &lut->hash_table[idx];
	for (;;) {
		struct lut_node *node = *nodepp;
		if (node == NULL) {
			ret = new_lut_node(lut, nodepp, key, keylen, data,
				datalen);
			break;
		}

		/* already in our hash table */
		if (lutkey_equal(lut, key, node->key) == true) {
			ret = 1;
			break;
		}

		nodepp = &(node->link);
	}
	return ret;
}

int aoc_lut_lookup(struct aoc_lut *lut, void *key, size_t keylen, 
	void *data, size_t datalen)
{
	int ret = -1;
	unsigned long idx;
	struct lut_node **nodepp;
	assert(lut != NULL);
	assert(key != NULL);
	assert(keylen == lut->keylen);
	if (datalen > 0) {
		assert(data != NULL);
		assert(datalen == lut->datalen);
	}

	/* generate a hash table index from our key and normalize it to 
	 * our hash table size */
	idx = fnv1a(key, keylen);
	idx = idx & ((1 << lut->shift) - 1);

	nodepp = &lut->hash_table[idx];
	for (;;) {
		struct lut_node *node = *nodepp;
		if (node == NULL) {
			break;
		}

		/* found in our hash table */
		if (lutkey_equal(lut, key, node->key) == true) {
			memcpy(data, node->data, datalen);
			ret = 0;
			break;
		}

		nodepp = &(node->link);
	}
	return ret;
}

int aoc_lut_remove(struct aoc_lut *lut, void *key, size_t keylen)
{
	int ret = -1;
	unsigned long idx;
	struct lut_node **nodepp;
	assert(lut != NULL);
	assert(key != NULL);
	assert(keylen == lut->keylen);

	/* generate a hash table index from our key and normalize it to 
	 * our hash table size */
	idx = fnv1a(key, keylen);
	idx = idx & ((1 << lut->shift) - 1);

	nodepp = &lut->hash_table[idx];
	for (;;) {
		struct lut_node *node = *nodepp;
		if (node == NULL) {
			break;
		}

		/* found in our hash table */
		if (lutkey_equal(lut, key, node->key) == true) {
			struct lut_node *tmp = *nodepp;
			*nodepp = tmp->link;
			free_lut_node(tmp);
			ret = 0;
			break;
		}

		nodepp = &(node->link);
	}
	return ret;
}

#if 0
struct aoc_lut {
	size_t shift;
	size_t size;
	size_t data_size;
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

struct aoc_lut *aoc_new_lut(size_t shift, size_t data_size, aoc_hashfn hashfn)
{
	size_t size;
	struct aoc_lut *lut;
	assert(shift > 0);
	
	size = sizeof(struct aoc_lut) + (sizeof(struct aoc_lut_node *) * (1 << shift));
	if ((lut = malloc(size)) == NULL)
		return NULL;

	lut->shift = shift;
	lut->size = 1 << shift;
	lut->data_size = data_size;
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
	if ((node = malloc(sizeof * node + (lut->data_size))) != NULL) {
		node->key = key;
		node->lut = lut;
		node->link = NULL;
		memset(node->data, 0, lut->data_size);
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

int aoc_lut_node_idx(struct aoc_lut *lut, struct aoc_lut_node **nodepp,
	size_t i)
{
	int ret;
	assert(lut != NULL);
	assert(nodepp != NULL);
	ret = -1;
	if (i < lut->size) {
		*nodepp = lut->table[i];
		ret = 0;
	}
	return ret;
}

void *aoc_lut_node_data(struct aoc_lut_node *node)
{
	assert(node != NULL);
	return (void*)(&node->data[0]);
}
#endif

