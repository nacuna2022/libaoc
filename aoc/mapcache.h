#ifndef __AOC_MAP_CACHE_H__
#define __AOC_MAP_CACHE_H__

/* advent of code uses a lot of maps where we need to 
 * process up, down, left and right. indexes with maps just dont make sense
 * anymore - so we don't use them here.
 */

struct aoc_mapcache;

struct aoc_mapcache *aoc_new_mapcache(char *pathname);
void aoc_free_mapcache(struct aoc_mapcache *cache);

/* determine what kind of tile we are stepping on */
int aoc_mapcache_tile(struct aoc_mapcache *cache, unsigned long *tile_id);

/* set current tile as starting point */
void aoc_mapcache_set_start(struct aoc_mapcache *cache);
void aoc_mapcache_reset(struct aoc_mapcache *cache);

/* sometimes, we need to "walk" the entire map too look for something - like
 * to look for the starting point. "walking" has the effect of 
 * going to the next level or row when you step off the map on its side. */
int aoc_mapcache_walk_forward(struct aoc_mapcache *cache);
int aoc_mapcache_walk_backward(struct aoc_mapcache *cache);

/* we also provide a "step" which is left/righ, top,bottom aware. */
int aoc_mapcache_step_up(struct aoc_mapcache *cache);
int aoc_mapcache_step_down(struct aoc_mapcache *cache);
int aoc_mapcache_step_left(struct aoc_mapcache *cache);
int aoc_mapcache_step_right(struct aoc_mapcache *cache);

/* sometimes we just want to "peek" at a direction */
int aoc_mapcache_peek_up(struct aoc_mapcache *cache);
int aoc_mapcache_peek_down(struct aoc_mapcache *cache);
int aoc_mapcache_peek_left(struct aoc_mapcache *cache);
int aoc_mapcache_peek_right(struct aoc_mapcache *cache);

/* for when we need the coord of the current tile */
void aoc_mapcache_coord(struct aoc_mapcache *cache, int *x, int *y);

/* for when we need to jump to a tile we visited prior */
int aoc_mapcache_goto_tile(struct aoc_mapcache *cache, unsigned long tile_id);

/* for when we need to change a tile to something else */
void aoc_mapcache_change_tile(struct aoc_mapcache *cache, int tile);

#endif /* __AOC_MAP_CACHE_H__ */

