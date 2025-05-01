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

static int hash_chain_item_count(struct lut_node *node)
{
	int count = 0;
	struct lut_node *curnode;
	if (node != NULL) {
		for (curnode = node; curnode != NULL; curnode = curnode->link) 
			count += 1;
	}
	return count;
}

int aoc_lut_item_count(struct aoc_lut *lut)
{
	int i;
	int count = 0;
	if (lut == NULL)
		return -1;
	for (i = 0; i < (1 << lut->shift); i += 1) {
		if (lut->hash_table[i] != NULL) {
			count += hash_chain_item_count(lut->hash_table[i]);
		}
	}
	return count;
}

static void hash_chain_item_callback(struct lut_node *node,
	aoc_lut_callback cb, void *cb_param)
{
	struct lut_node *curnode;
	if (node != NULL) {
		for (curnode = node; curnode != NULL; curnode = curnode->link) {
			cb(curnode->key, curnode->data, cb_param);
		}
	}
	return;
}

int aoc_lut_foreach(struct aoc_lut *lut, aoc_lut_callback cb, void *cb_param)
{
	int i;
	if (lut == NULL)
		return -1;
	if (cb == NULL)
		return -1;
	
	for (i = 0; i < (1 << lut->shift); i += 1) {
		if (lut->hash_table[i] != NULL) {
			hash_chain_item_callback(lut->hash_table[i],
				cb, cb_param);
		}
	}
	
	return 0;
}
