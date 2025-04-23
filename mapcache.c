#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include <aoc/mapcache.h>
#include <aoc/incache.h>

struct aoc_mapcache {
	int linesize;
	int size;
	char *start;
	char *pos;
	char data[];
};

static void init_mapcache(struct aoc_mapcache *cache,
	struct aoc_incache *incache)
{
	bool linesize_found = false;
	int size = 0;
	int linesize;
	int ch;
	int idx = 0;
	while((ch = aoc_incache_get(incache, idx++)) != -1) {
		if (ch == '\n') {
			if (!linesize_found) {
				linesize = size;
				linesize_found = true;
			}
			continue;
		}
		cache->data[size] = (char)ch;
		size += 1;
	}
	cache->size = size;
	cache->linesize = linesize;
	cache->pos = &cache->data[0];
	aoc_mapcache_set_start(cache);
	return;
}

static struct aoc_mapcache *new_mapcache(size_t mapsize)
{
	return malloc(sizeof(struct aoc_mapcache) + mapsize);
}

struct aoc_mapcache *aoc_new_mapcache(char *pathname)
{
	struct aoc_incache *incache;
	struct aoc_mapcache *cache;
	size_t rawsize;
	incache = aoc_new_incache(pathname);
	if (incache == NULL)
		return NULL;
	rawsize = aoc_incache_size(incache);
	cache = new_mapcache(rawsize);
	if (cache != NULL) {
		init_mapcache(cache, incache);
	}
	aoc_free_incache(incache);
	return cache;
}

struct aoc_mapcache *aoc_new_mapcache_grid(int row, int col, int tile)
{
	struct aoc_mapcache *cache;
	assert(row > 0);
	assert(col > 0);
	if ((cache = new_mapcache(row * col)) != NULL) {
		memset(cache->data, tile, row * col);
		cache->linesize = col;
		cache->pos = &cache->data[0];
		cache->size = row * col;
		aoc_mapcache_set_start(cache);
	}
	return cache;
}

void aoc_free_mapcache(struct aoc_mapcache *cache)
{
	assert(cache != NULL);
	free(cache);
	return;
}

void aoc_mapcache_set_start(struct aoc_mapcache *cache)
{
	assert(cache != NULL);
	cache->start = cache->pos;
	return;
}

void aoc_mapcache_reset(struct aoc_mapcache *cache)
{
	assert(cache != NULL);
	cache->pos = cache->start;
	return;
}

static bool out_of_bounds(struct aoc_mapcache *cache, char *pos)
{
	char *start = cache->data;
	char *end = cache->data + cache->size;
	if ((pos < start) || (pos > (end-1))) {
		return true;
	} 
	return false;
}

static char *offset_pos(struct aoc_mapcache *cache, int offset)
{
	char *ptr = cache->pos + offset;
	if (out_of_bounds(cache, ptr)) {
		return NULL;
	}
	return ptr;
}

static int walk(struct aoc_mapcache *cache, int offset)
{
	char *ptr;
	int ch = -1;
	ptr = offset_pos(cache, offset);
	if (ptr != NULL) {
		cache->pos = ptr;
		ch = aoc_mapcache_tile(cache, NULL);
	}
	return ch;
}

int aoc_mapcache_walk_forward(struct aoc_mapcache *cache)
{
	assert(cache != NULL);
	return walk(cache, 1);
}

int aoc_mapcache_walk_backward(struct aoc_mapcache *cache)
{
	assert(cache != NULL);
	return walk(cache, -1);
}

static int get_tile(struct aoc_mapcache *cache,	unsigned long *tile_id,
	char *pos)
{
	assert(cache != NULL);
	if (tile_id) {
		*tile_id = (unsigned long)cache->pos;
	}
	return (int)(*pos & 0xFF);
}

int aoc_mapcache_tile(struct aoc_mapcache *cache, unsigned long *tile_id)
{
	assert(cache != NULL);
	return get_tile(cache, tile_id, cache->pos);
}

static char *peek_vertical(struct aoc_mapcache *cache, bool up)
{
	int offset;
	offset = up == true ? -cache->linesize : cache->linesize;
	return offset_pos(cache, offset);
}

static int step_vertical(struct aoc_mapcache *cache, bool up)
{
	char *ptr;
	assert(cache != NULL);
	ptr = peek_vertical(cache, up);
	if (ptr == NULL)
		return -1;
	cache->pos = ptr;
	return aoc_mapcache_tile(cache, NULL);
}

static char *offset_pos2(struct aoc_mapcache *cache, int offset)
{
	int leftmost_idx;
	int rightmost_idx;
	int cur_idx;
	int offset_idx;
	
	cur_idx = (int)(cache->pos - cache->data);
	offset_idx = cur_idx + offset;
	
	/* we reference leftmost and rightmost idx on cur_idx */
	leftmost_idx = (cur_idx / cache->linesize) * cache->linesize;
	rightmost_idx = ((cur_idx + cache->linesize) / cache->linesize) * 
		cache->linesize;

	/* leftmost_idx <= offset_idx < rightmost_idx */
	if ((leftmost_idx <= offset_idx) && (offset_idx < rightmost_idx)) {
		return cache->pos + offset;
	}
	return NULL;
}

static char *peek_horizontal(struct aoc_mapcache *cache, bool left)
{
	int offset;
	offset = left == true ? -1 : 1;
	return offset_pos2(cache, offset);
}

static int step_horizontal(struct aoc_mapcache *cache, bool left)
{
	char *ptr;
	assert(cache != NULL);
	ptr = peek_horizontal(cache, left);
	if (ptr == NULL)
		return -1;
	cache->pos = ptr;
	return aoc_mapcache_tile(cache, NULL);
}

int aoc_mapcache_step_up(struct aoc_mapcache *cache)
{
	return step_vertical(cache, true);
}

int aoc_mapcache_step_down(struct aoc_mapcache *cache)
{
	return step_vertical(cache, false);
}

int aoc_mapcache_step_left(struct aoc_mapcache *cache)
{
	return step_horizontal(cache, true);
}

int aoc_mapcache_step_right(struct aoc_mapcache *cache)
{
	return step_horizontal(cache, false);
}

int aoc_mapcache_peek_up(struct aoc_mapcache *cache)
{
	char *ptr;
	assert(cache != NULL);
	ptr = peek_vertical(cache, true);
	if (ptr != NULL) {
		return get_tile(cache, NULL, ptr);
	}
	return -1;
}

int aoc_mapcache_peek_down(struct aoc_mapcache *cache)
{
	char *ptr;
	assert(cache != NULL);
	ptr = peek_vertical(cache, false);
	if (ptr != NULL) {
		return get_tile(cache, NULL, ptr);
	}
	return -1;
}

int aoc_mapcache_peek_left(struct aoc_mapcache *cache)
{
	char *ptr;
	assert(cache != NULL);
	ptr = peek_horizontal(cache, true);
	if (ptr != NULL) {
		return get_tile(cache, NULL, ptr);
	}
	return -1;
}

int aoc_mapcache_peek_right(struct aoc_mapcache *cache)
{
	char *ptr;
	assert(cache != NULL);
	ptr = peek_horizontal(cache, false);
	if (ptr != NULL) {
		return get_tile(cache, NULL, ptr);
	}
	return -1;
}

void aoc_mapcache_coord(struct aoc_mapcache *cache, int *x, int *y)
{
	unsigned long idx;
	assert(cache != NULL);
	assert(x != NULL);
	assert(y != NULL);
	idx = (unsigned long)cache->pos - (unsigned long)cache->start;
	*x = idx / cache->linesize;
	*y = idx % cache->linesize;
	return;
}

int aoc_mapcache_goto_tile(struct aoc_mapcache *cache, unsigned long tile_id)
{
	unsigned long id;
	int tile;
	assert(cache != NULL);
	
	/* change the start point because we need to walk the map */
	cache->pos = &cache->data[0];
	for (;;) {
		tile = aoc_mapcache_tile(cache, &id);
		if (id == tile_id)
			return tile;
		
		if (aoc_mapcache_walk_forward(cache) == -1)
			break;
	}
	return -1;
}

void aoc_mapcache_change_tile(struct aoc_mapcache *cache, int tile)
{
	assert(cache != NULL);
	*(cache->pos) = ((char)tile) & 0xFF;
	return;
}

int aoc_mapcache_warp_up(struct aoc_mapcache *cache)
{
	int tile;
	assert(cache != NULL);
	tile = aoc_mapcache_step_up(cache);
	if (tile == -1) {
		/* if we cannot go up, that means we need to warp down */
		char *ptr;
		ptr = (cache->pos + cache->size) - cache->linesize;
		cache->pos = ptr;
		tile = aoc_mapcache_tile(cache, NULL);
		assert(aoc_mapcache_step_down(cache) == -1);
	}
	return tile;
}

int aoc_mapcache_warp_down(struct aoc_mapcache *cache)
{
	int tile;
	assert(cache != NULL);
	tile = aoc_mapcache_step_down(cache);
	if (tile == -1) {
		/* if we cannot go down, that means we need to warp up */
		char *ptr;
		ptr = (cache->pos - cache->size) + cache->linesize;
		cache->pos = ptr;
		tile = aoc_mapcache_tile(cache, NULL);
		assert(aoc_mapcache_step_up(cache) == -1);
	}
	return tile;
}

int aoc_mapcache_warp_left(struct aoc_mapcache *cache)
{
	int tile;
	assert(cache != NULL);
	tile = aoc_mapcache_step_left(cache);
	if (tile == -1) {
		/* if we cannot go left, that means we need to warp right */
		char *ptr;
		ptr = cache->pos + (cache->linesize - 1);
		cache->pos = ptr;
		tile = aoc_mapcache_tile(cache, NULL);
		assert(aoc_mapcache_step_right(cache) == -1);
	}
	return tile;
}

int aoc_mapcache_warp_right(struct aoc_mapcache *cache)
{
	int tile;
	assert(cache != NULL);
	tile = aoc_mapcache_step_right(cache);
	if (tile == -1) {
		/* if we cannot go up, that means we need to warp left */
		char *ptr;
		ptr = cache->pos - (cache->linesize - 1);
		cache->pos = ptr;
		tile = aoc_mapcache_tile(cache, NULL);
		assert(aoc_mapcache_step_left(cache) == -1);
	}
	return tile;
}

int aoc_mapcache_size(struct aoc_mapcache *cache)
{
	assert(cache != NULL);
	return cache->size;
}

int aoc_mapcache_width(struct aoc_mapcache *cache)
{
	assert(cache != NULL);
	return cache->linesize;
}

int aoc_mapcache_height(struct aoc_mapcache *cache)
{
	assert(cache != NULL);
	return aoc_mapcache_size(cache) / aoc_mapcache_width(cache);
}

void aoc_mapcache_show(struct aoc_mapcache *cache)
{
	int i;
	assert(cache != NULL);
	for (i = 0; i < cache->size; i += 1) {
		if ((i % cache->linesize) == 0)
			printf("\n");
		printf("%c", cache->data[i]);
	}
	printf("\n");
	return;
}

struct aoc_mapcache *aoc_mapcache_dup(struct aoc_mapcache *cache)
{
	struct aoc_mapcache *cache_dup;
	int x;
	int y;
	assert(cache != NULL);
	x = cache->size / cache->linesize;
	y = cache->linesize;
	cache_dup = aoc_new_mapcache_grid(x, y, 0);
	if (cache_dup != NULL) {
		memcpy(cache_dup->data, cache->data, cache->size);
	}
	return cache_dup;
}

int aoc_mapcache_find_marker(struct aoc_mapcache *cache, int tile)
{
	char *save_pos;
	assert(cache != NULL);

	/* save the current position in case we did not find the marker tile */
	save_pos = cache->pos;

	for (;;) {
		if (aoc_mapcache_walk_forward(cache) == -1) {
			break;
		}

		int what = aoc_mapcache_tile(cache, NULL);
		if (what == tile) {
			return what;
		}
	}

	cache->pos = save_pos;
	return - 1;
}

int aoc_mapcache_peek_dir(struct aoc_mapcache *cache, enum aoc_direction dir)
{
	assert(cache != NULL);
	switch(dir) {
	case aoc_direction_up:
		return aoc_mapcache_peek_up(cache);
	case aoc_direction_right:
		return aoc_mapcache_peek_right(cache);
	case aoc_direction_down:
		return aoc_mapcache_peek_down(cache);
	case aoc_direction_left:
		return aoc_mapcache_peek_left(cache);
	default:
		assert(0);
		break;
	}
	/* won't reach here */
	assert(0);
	return -1;
}

int aoc_mapcache_step_dir(struct aoc_mapcache *cache, enum aoc_direction dir)
{
	assert(cache != NULL);
	switch(dir) {
	case aoc_direction_up:
		return aoc_mapcache_step_up(cache);
	case aoc_direction_right:
		return aoc_mapcache_step_right(cache);
	case aoc_direction_down:
		return aoc_mapcache_step_down(cache);
	case aoc_direction_left:
		return aoc_mapcache_step_left(cache);
	default:
		assert(0);
		break;
	}
	/* won't reach here */
	assert(0);
	return -1;
}

int aoc_mapcache_warp_dir(struct aoc_mapcache *cache, enum aoc_direction dir)
{
	assert(cache != NULL);
	switch(dir) {
	case aoc_direction_up:
		return aoc_mapcache_warp_up(cache);
	case aoc_direction_right:
		return aoc_mapcache_warp_right(cache);
	case aoc_direction_down:
		return aoc_mapcache_warp_down(cache);
	case aoc_direction_left:
		return aoc_mapcache_warp_left(cache);
	default:
		assert(0);
		break;
	}
	/* won't reach here */
	assert(0);
	return -1;
}

