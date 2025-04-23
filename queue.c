#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <aoc/queue.h>
#include <aoc/dlist.h>

struct queue_node {
	struct aoc_dlist_node node;
	char __attribute__((aligned(8))) data[0];
};

struct aoc_queue {
	size_t data_size;
	struct aoc_dlist_node list;
};

struct aoc_queue *aoc_new_queue(size_t data_size)
{
	struct aoc_queue *q;
	if ((q = malloc(sizeof(struct aoc_queue))) != NULL) {
		q->data_size = data_size;
		aoc_dlist_init(&q->list);
	}
	return q;
}

static int dequeue_one(struct aoc_queue *q, void *dst)
{
	int ret = -1;
	assert(q != NULL);
	if (q->list.next != &q->list) {
		struct queue_node *qn;
		struct aoc_dlist_node *node = q->list.next;
		qn = aoc_dlist_container(node, offsetof(struct queue_node, node));
		aoc_dlist_remove(&qn->node);
		if (dst) {
			mempcpy(dst, qn->data, q->data_size);
		}
		free(qn);
		ret = 0;
	}
	return ret;
}

void aoc_free_queue(struct aoc_queue *q)
{
	assert(q != NULL);
	for (;;) {
		if (dequeue_one(q, NULL) == -1) {
			break;
		}
	}
	free(q);
	return;
}

static struct queue_node *new_queue_node(struct aoc_queue *q,
	void *data)
{
	struct queue_node *qn;
	size_t node_size = sizeof(struct queue_node) + q->data_size;
	if ((qn = malloc(node_size)) != NULL) {
		memcpy(qn->data, data, q->data_size);
	}
	return qn;
}

int aoc_queue(struct aoc_queue *q, void *src, size_t src_size)
{
	int ret = -1;
	struct queue_node *qn;
	assert(q != NULL);
	assert(src != NULL);
	assert(src_size == q->data_size);
	if ((qn = new_queue_node(q, src)) != NULL) {
		aoc_dlist_prepend(&q->list, &qn->node);
		ret = 0;
	}
	return ret;
}

int aoc_dequeue(struct aoc_queue *q, void *dst, size_t dst_size)
{
	assert(q != NULL);
	assert(dst != NULL);
	assert(dst_size == q->data_size);
	return dequeue_one(q, dst);
}

