#ifndef __AOC_MAP_CACHE_H__
#define __AOC_MAP_CACHE_H__

#include <aoc/dir.h>
 
/* advent of code uses a lot of maps where we need to 
 * process up, down, left and right. indexes with maps just dont make sense
 * anymore - so we don't use them here.
 */

struct aoc_mapcache;

struct aoc_mapcache *aoc_new_mapcache(char *pathname);

/* sometimes, we want to make a mapcache without any input file backing it */
struct aoc_mapcache *aoc_new_mapcache_grid(int row, int col, int tile);

void aoc_free_mapcache(struct aoc_mapcache *cache);

/* mapcache dimensions */
int aoc_mapcache_size(struct aoc_mapcache *cache);
int aoc_mapcache_width(struct aoc_mapcache *cache);
int aoc_mapcache_height(struct aoc_mapcache *cache);

/* determine what kind of tile we are stepping on */
int aoc_mapcache_tile(struct aoc_mapcache *cache, unsigned long *tile_id);

/* set current tile as starting point */
void aoc_mapcache_set_start(struct aoc_mapcache *cache);
void aoc_mapcache_reset(struct aoc_mapcache *cache);

/* sometimes, we want an "absolute" reset where the starting point is 
 * tile (0,0), the first tile */
void aoc_mapcache_absolute_reset(struct aoc_mapcache *cache);

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

/* add support for parameterized step direction */
int aoc_mapcache_step_dir(struct aoc_mapcache *cache, enum aoc_direction dir);

/* we can also "warp" which is basically stepping off from the side and 
 * appearing on the other side. */
int aoc_mapcache_warp_up(struct aoc_mapcache *cache);
int aoc_mapcache_warp_down(struct aoc_mapcache *cache);
int aoc_mapcache_warp_left(struct aoc_mapcache *cache);
int aoc_mapcache_warp_right(struct aoc_mapcache *cache);

/* add support for parameterized warp direction */
int aoc_mapcache_warp_dir(struct aoc_mapcache *cache, enum aoc_direction dir);

/* sometimes we just want to "peek" at a direction */
int aoc_mapcache_peek_up(struct aoc_mapcache *cache);
int aoc_mapcache_peek_down(struct aoc_mapcache *cache);
int aoc_mapcache_peek_left(struct aoc_mapcache *cache);
int aoc_mapcache_peek_right(struct aoc_mapcache *cache);

/* add support for parameterized peek direction */
int aoc_mapcache_peek_dir(struct aoc_mapcache *cache, enum aoc_direction dir);

/* for when we need the coord of the current tile */
void aoc_mapcache_coord(struct aoc_mapcache *cache, int *x, int *y);

/* for when we need to jump to a tile we visited prior */
int aoc_mapcache_goto_tile(struct aoc_mapcache *cache, unsigned long tile_id);

/* for when we need to change a tile to something else */
void aoc_mapcache_change_tile(struct aoc_mapcache *cache, int tile);

/* for when we want to print the map */
void aoc_mapcache_show(struct aoc_mapcache *cache);

/* for when we want to create a copy of the mapcache */
struct aoc_mapcache *aoc_mapcache_dup(struct aoc_mapcache *cache);

/* sometimes, a map has a unique "marker" tile were walkers like robot will
 * start walking from. we use this to set the internal map pointer to that 
 * tile. this will stop on the first encounter of that tile. */
int aoc_mapcache_find_marker(struct aoc_mapcache *cache, int tile);


#endif /* __AOC_MAP_CACHE_H__ */

