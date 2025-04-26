#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <aoc/minheap.h>

struct minheap_node {
	int key;
	void *payload[0];
};

#if 0
given a parent index i, we can locate its:
	left child  : li = 2i + 1
	right child : ri = 2i + 2

given a child node i, we can locate its parent:
	pi = (i - 1) >> 1
#endif

struct aoc_minheap {
#define HEAP_GROW_SIZE 	1
	size_t size;
	size_t count;
	size_t payload_size;
	struct minheap_node *heap[0];
};

static void init_heap(struct aoc_minheap *heap, size_t payload_size)
{
	assert(heap != NULL);
	heap->size = HEAP_GROW_SIZE;
	heap->count = 0;
	heap->payload_size = payload_size;
	return;
}

struct aoc_minheap *aoc_new_minheap(size_t payload_size)
{
	struct aoc_minheap *heap;
	size_t heap_size = sizeof(struct minheap_node *) * HEAP_GROW_SIZE;
	if ((heap = malloc(sizeof(struct aoc_minheap) + heap_size))) {
		init_heap(heap, payload_size);
	}
	return heap;
}


static void free_all_heap_nodes(struct aoc_minheap *heap)
{
	assert(heap != NULL);
	int key;
	for (;;) {
		if (aoc_minheap_get(heap, &key, NULL, 0) == -1) {
			break;
		}
	}
	return;
}

void aoc_free_minheap(struct aoc_minheap *minheap)
{
	assert(minheap != NULL);
	free_all_heap_nodes(minheap);
	free(minheap);
	return;
}

static struct minheap_node *new_minheap_node(int key, void *payload,
	size_t payload_size)
{
	struct minheap_node *node;
	if ((node = malloc(sizeof(struct minheap_node) + payload_size)) != NULL) {
		node->key = key;
		if (payload != NULL) {
			memcpy(node->payload, payload, payload_size);
		}
	}
	return node;
}

static struct aoc_minheap *enlarge_minheap(struct aoc_minheap *heap)
{
	struct aoc_minheap *tmp_heap;
	assert(heap != NULL);
	size_t new_size = sizeof(struct minheap_node *) * (heap->size + HEAP_GROW_SIZE);
	if ((tmp_heap = realloc(heap, sizeof(struct aoc_minheap) + new_size)) != NULL) {
		tmp_heap->size += HEAP_GROW_SIZE;
	}
	return tmp_heap;
}

static void heapify_up(struct aoc_minheap *heap)
{
	size_t last_idx;
	assert(heap != NULL);
	last_idx = heap->count - 1;
	while(last_idx > 0) {
		size_t pi;
		struct minheap_node *parent;
		struct minheap_node *child;
		struct minheap_node *tmp;

		/* get parent */
		pi = (last_idx - 1) >> 1;
		child = heap->heap[last_idx];
		parent = heap->heap[pi];

		/* if child is greater than parent, we need to swap them 
		 * to satisfy the heap property */
		if (parent->key > child->key) {
			tmp = parent;
			heap->heap[pi] = child;
			heap->heap[last_idx] = tmp;

			/* this must be obeyed all throughout the heap,
			 * so we need to go up the heap tree */
			last_idx = pi;
			continue;
		}
		break;
	}
	return;
}

static void insert_node(struct aoc_minheap **heap, struct minheap_node *node)
{
	assert(heap != NULL);
	assert(node != NULL);

	/* enlarge the heap if we need to */
	while ((*heap)->count >= (*heap)->size) {
		struct aoc_minheap *new_minheap;
		new_minheap = enlarge_minheap(*heap);
		assert(new_minheap != NULL);
		if (new_minheap != *heap)
			*heap = new_minheap;
	}

	struct aoc_minheap *h = *heap;
	/* now we can insert at the last */
	h->heap[h->count] = node;
	h->count += 1;

	/* lastly, heapify upwards */
	heapify_up(h);

	return;
}

int aoc_minheap_insert(struct aoc_minheap **minheap, int key, void *payload, 
	size_t payload_size)
{
	int err = -1;
	struct minheap_node *node;
	assert(minheap != NULL);

	if (payload != NULL) 
		assert((*minheap)->payload_size <= payload_size);
	
	node = new_minheap_node(key, payload, (*minheap)->payload_size);
	if (node != NULL) {
		insert_node(minheap, node);
		err = 0;
	}
	return err;
}

static void heapify_down(struct aoc_minheap *heap)
{
	size_t pi = 0;
	size_t li;
	size_t ri;

	assert(heap != NULL);
	for(;;) {
		struct minheap_node *smallest;
		struct minheap_node *parent;
		struct minheap_node *left = NULL;
		struct minheap_node *right = NULL;
		
		li = (2 * pi) + 1;
		ri = (2 * pi) + 2;
	
		parent = heap->heap[pi];
		assert(parent != NULL);

		if (li < heap->count) 
			left = heap->heap[li];

		if (ri < heap->count )
			right = heap->heap[ri];

		/* parent is initially assumed to be the smallest */
		smallest = parent;

		if ((left != NULL) && (left->key < smallest->key)) {
			smallest = left;
		}

		if ((right != NULL) && (right->key < smallest->key)) {
			smallest  = right;
		}

		/* if it turns out the parent is the smallest, then we 
		 * are done. this can happen especially if parent currently 
		 * has no child */
		if (smallest == parent) {
			break;
		}

		/* swap parent with smallest */
		struct minheap_node *tmp = heap->heap[pi];
		if (smallest == left) {
			heap->heap[pi] = heap->heap[li];
			heap->heap[li] = tmp;
			pi = li;
		} else {
			heap->heap[pi] = heap->heap[ri];
			heap->heap[ri] = tmp;
			pi = ri;
		}
	}
	return;
}

int aoc_minheap_get(struct aoc_minheap *heap, int *key, void *payload,
	size_t payload_size)
{
	struct minheap_node *node;
	assert(heap != NULL);

	if (aoc_minheap_peek(heap, key, payload, payload_size) == -1) {
		return -1;
	}

	node = heap->heap[0];
	free(node);
	heap->heap[0] = heap->heap[heap->count - 1];
	heap->count -= 1;
	heapify_down(heap);
	return 0;
}

int aoc_minheap_peek(struct aoc_minheap *heap, int *key, void *payload,
	size_t payload_size)
{
	struct minheap_node *node;
	assert(heap != NULL);
	assert(key != NULL);
	if (heap->count < 1) {
		return -1;
	}
	node = heap->heap[0];
	*key = node->key;
	if (payload_size) {
		assert(payload != NULL);
		assert(payload_size <= heap->payload_size);
		memcpy(payload, node->payload, heap->payload_size);
	}
	return 0;
}

