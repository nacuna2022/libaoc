#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <aoc/priority_queue.h>

struct prio_queue_node {
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

struct aoc_priority_queue {
#define HEAP_GROW_SIZE 	1
	size_t size;
	size_t count;
	size_t payload_size;
	struct prio_queue_node *heap[0];
};

static void init_priority_queue(struct aoc_priority_queue *prio_queue,
	size_t payload_size)
{
	assert(prio_queue != NULL);
	prio_queue->size = HEAP_GROW_SIZE;
	prio_queue->count = 0;
	prio_queue->payload_size = payload_size;
	return;
}

struct aoc_priority_queue *aoc_new_priority_queue(size_t payload_size)
{
	struct aoc_priority_queue *heap;
	size_t heap_size = sizeof(struct prio_queue_node *) * HEAP_GROW_SIZE;
	if ((heap = malloc(sizeof(struct aoc_priority_queue) + heap_size))) {
		init_priority_queue(heap, payload_size);
	}
	return heap;
}


static void free_all_heap_nodes(struct aoc_priority_queue *heap)
{
	assert(heap != NULL);
	int key;
	for (;;) {
		if (aoc_priority_queue_get_min(heap, &key, NULL, 0) == -1) {
			break;
		}
	}
	return;
}

void aoc_free_priority_queue(struct aoc_priority_queue *prio_queue)
{
	assert(prio_queue != NULL);
	free_all_heap_nodes(prio_queue);
	free(prio_queue);
	return;
}

static struct prio_queue_node *new_priority_queue_node(int key, void *payload,
	size_t payload_size)
{
	struct prio_queue_node *node;
	if ((node = malloc(sizeof(struct prio_queue_node) + payload_size)) != NULL) {
		node->key = key;
		if (payload != NULL) {
			memcpy(node->payload, payload, payload_size);
		}
	}
	return node;
}

static struct aoc_priority_queue *enlarge_minheap(struct aoc_priority_queue *heap)
{
	struct aoc_priority_queue *tmp_heap;
	assert(heap != NULL);
	size_t new_size = sizeof(struct prio_queue_node *) * (heap->size + HEAP_GROW_SIZE);
	if ((tmp_heap = realloc(heap, sizeof(struct aoc_priority_queue) + new_size)) != NULL) {
		tmp_heap->size += HEAP_GROW_SIZE;
	}
	return tmp_heap;
}

static void heapify_up(struct aoc_priority_queue *heap)
{
	size_t last_idx;
	assert(heap != NULL);
	last_idx = heap->count - 1;
	while(last_idx > 0) {
		size_t pi;
		struct prio_queue_node *parent;
		struct prio_queue_node *child;
		struct prio_queue_node *tmp;

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

static void insert_node(struct aoc_priority_queue **prio_queue,
	struct prio_queue_node *node)
{
	assert(prio_queue != NULL);
	assert(node != NULL);

	/* enlarge the prio_queue if we need to */
	while ((*prio_queue)->count >= (*prio_queue)->size) {
		struct aoc_priority_queue *new_minheap;
		new_minheap = enlarge_minheap(*prio_queue);
		assert(new_minheap != NULL);
		if (new_minheap != *prio_queue)
			*prio_queue = new_minheap;
	}

	struct aoc_priority_queue *h = *prio_queue;
	/* now we can insert at the last */
	h->heap[h->count] = node;
	h->count += 1;

	/* lastly, heapify upwards */
	heapify_up(h);

	return;
}

int aoc_priority_queue_insert(struct aoc_priority_queue **prio_queue, int key,
	void *payload, size_t payload_size)
{
	int err = -1;
	struct prio_queue_node *node;
	assert(prio_queue != NULL);

	if (payload != NULL) 
		assert((*prio_queue)->payload_size <= payload_size);
	
	node = new_priority_queue_node(key, payload, (*prio_queue)->payload_size);
	if (node != NULL) {
		insert_node(prio_queue, node);
		err = 0;
	}
	return err;
}

static void heapify_down(struct aoc_priority_queue *heap)
{
	size_t pi = 0;
	size_t li;
	size_t ri;

	assert(heap != NULL);
	for(;;) {
		struct prio_queue_node *smallest;
		struct prio_queue_node *parent;
		struct prio_queue_node *left = NULL;
		struct prio_queue_node *right = NULL;
		
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
		struct prio_queue_node *tmp = heap->heap[pi];
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

int aoc_priority_queue_get_min(struct aoc_priority_queue *heap,	int *key,
	void *payload, size_t payload_size)
{
	struct prio_queue_node *node;
	assert(heap != NULL);

	if (aoc_priority_queue_peek(heap, key, payload, payload_size) == -1) {
		return -1;
	}

	node = heap->heap[0];
	free(node);
	heap->heap[0] = heap->heap[heap->count - 1];
	heap->count -= 1;
	heapify_down(heap);
	return 0;
}

int aoc_priority_queue_peek(struct aoc_priority_queue *heap, int *key,
	void *payload, size_t payload_size)
{
	struct prio_queue_node *node;
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

