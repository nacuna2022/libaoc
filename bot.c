#include <stdlib.h>
#include <assert.h>

#include <aoc/bot.h>

struct aoc_bot {
	enum aoc_direction facing;
};

struct aoc_bot *aoc_new_bot(enum aoc_direction cardinal_dir)
{
	struct aoc_bot *bot;
	if ((bot = malloc(sizeof * bot))) {
		aoc_bot_init(bot, cardinal_dir);
	}
	return bot;
}

void aoc_free_bot(struct aoc_bot *bot)
{
	free(bot);
	return;
}

enum aoc_direction aoc_bot_init(struct aoc_bot *bot,
	enum aoc_direction cardinal_dir)
{
	assert(bot != NULL);
	bot->facing = cardinal_dir;
	return bot->facing;
}

enum aoc_direction aoc_bot_turn_right(struct aoc_bot *bot)
{
	enum aoc_direction bot_right;
	assert(bot != NULL);
	bot_right = aoc_bot_get_right(bot);
	return aoc_bot_init(bot, bot_right);
}

enum aoc_direction aoc_bot_turn_left(struct aoc_bot *bot)
{
	enum aoc_direction bot_left;
	assert(bot != NULL);
	bot_left = aoc_bot_get_left(bot);
	return aoc_bot_init(bot, bot_left);
}

/* map the bot relative direction to cardinal directions */
enum aoc_direction aoc_bot_get_front(struct aoc_bot *bot)
{
	assert(bot != NULL);
	return bot->facing;
}

enum aoc_direction aoc_bot_get_left(struct aoc_bot *bot)
{
	int l;
	assert(bot != NULL);
	l = (bot->facing + 3) % aoc_direction_limit;
	return (enum aoc_direction)l;
}

enum aoc_direction aoc_bot_get_right(struct aoc_bot *bot)
{
	int r;
	assert(bot != NULL);
	r = (bot->facing + 1) % aoc_direction_limit;
	return (enum aoc_direction)r;
}

enum aoc_direction aoc_bot_get_rear(struct aoc_bot *bot)
{
	int b;
	assert(bot != NULL);
	b = (bot->facing + 2) % aoc_direction_limit;
	return (enum aoc_direction)b;
}

