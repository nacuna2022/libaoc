#ifndef __AOC_BOT_H__
#define __AOC_BOT_H__

/* some AOC puzzles (specifically those dealing with maps) require a "walker".
 * we define a "bot" that keeps track of the direction where it is facing 
 * (or going) and can map the relative direction to the cardinal direction. 
 *
 * e.g. if a bot is facing right (its forward direction), it's left is actually
 * the cardinal direction 'top', etc.
 *
 * the intention is to use this with mapcaches which work only with cardinal 
 * directions up, down, left & right.
 * */

enum aoc_direction {
	aoc_direction_up = 0,
	aoc_direction_right,
	aoc_direction_down,
	aoc_direction_left,
	aoc_direction_limit,
};

struct aoc_bot;

struct aoc_bot *aoc_new_bot(enum aoc_direction facing);
void aoc_free_bot(struct aoc_bot *bot);

enum aoc_direction aoc_bot_init(struct aoc_bot *bot,
	enum aoc_direction cardinal_dir);
enum aoc_direction aoc_bot_turn_right(struct aoc_bot *bot);
enum aoc_direction aoc_bot_turn_left(struct aoc_bot *bot);

/* map the bot relative direction to cardinal directions */
enum aoc_direction aoc_bot_get_front(struct aoc_bot *bot);
enum aoc_direction aoc_bot_get_left(struct aoc_bot *bot);
enum aoc_direction aoc_bot_get_right(struct aoc_bot *bot);
enum aoc_direction aoc_bot_get_rear(struct aoc_bot *bot);

#endif /* __AOC_BOT_H__ */

