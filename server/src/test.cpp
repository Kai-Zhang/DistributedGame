#include <cstdio>
#include <cassert>
#include "game.h"

player p1, p2;

int test_game_logic() {
	int miss = 0, critical = 0, physical = 0, magical = 0;
	p1.character = PRO_WARRIOR;
	p1.health_point = WARRIOR_MAX_HP;
	p1.magic_point = WARRIOR_MAX_MP;
	p1.defense = WARRIOR_DEFENSE;
	p1.strength = WARRIOR_STRENGTH;
	p1.speed = WARRIOR_SPEED;

	p2.character = PRO_ARCHER;
	p2.health_point = ARCHER_MAX_HP;
	p2.magic_point = ARCHER_MAX_MP;
	p2.defense = ARCHER_DEFENSE;
	p2.strength = ARCHER_STRENGTH;
	p2.speed = ARCHER_SPEED;

	int last_health_point = p2.health_point;
	while (true) {
		int rst = physical_attack(p1, p2);
		assert(p1.health_point == WARRIOR_MAX_HP);
		assert(p1.magic_point == WARRIOR_MAX_MP);
		assert(p2.magic_point == ARCHER_MAX_MP);
		physical ++;
	
		if (rst & EFFECT_MISS) {
			miss ++;
			assert(p2.health_point == ARCHER_MAX_HP);
		}
		else if (rst & EFFECT_CRITICAL_HIT) {
			critical ++;
			assert(p2.health_point == last_health_point - 2 * (WARRIOR_STRENGTH - ARCHER_DEFENSE));
		}
		else if (rst & EFFECT_SETTLED) {
			assert(p2.health_point <= 0);
			break;
		}
		else {
			assert(p2.health_point == last_health_point - (WARRIOR_STRENGTH - ARCHER_DEFENSE));
		}

		last_health_point = p2.health_point;
	}
	printf("Physical attack test end. In %d physical attacks, there're %d misses and %d critical attack.\n",
			physical, miss, critical);

	miss = 0; critical = 0;
	p2.character = PRO_ARCHER;
	p2.health_point = ARCHER_MAX_HP;
	p2.magic_point = ARCHER_MAX_MP;
	p2.defense = ARCHER_DEFENSE;
	p2.strength = ARCHER_STRENGTH;
	p2.speed = ARCHER_SPEED;

	last_health_point = p2.health_point;
	int last_magic_point = p1.magic_point;
	while (true) {
		int rst = magical_attack(p1, p2);
		assert(p1.health_point == WARRIOR_MAX_HP);
		assert(p2.magic_point == ARCHER_MAX_MP);
		magical ++;
	
		if (rst & EFFECT_MISS) {
			assert(p2.health_point == ARCHER_MAX_HP);
		}
		else if (rst & EFFECT_CRITICAL_HIT) {
			assert(p1.magic_point == last_magic_point - WARRIOR_MPCONSUME);
			assert(p2.health_point == last_health_point - (int)(1.5 * WARRIOR_STRENGTH));
		}
		else if (rst & EFFECT_SETTLED) {
			assert(p1.magic_point == last_magic_point - WARRIOR_MPCONSUME);
			assert(p2.health_point <= 0);
			break;
		}
		else {
			assert(p1.magic_point == last_magic_point - WARRIOR_MPCONSUME);
			assert(p2.health_point == last_health_point - WARRIOR_STRENGTH);
		}

		last_health_point = p2.health_point;
		last_magic_point = p1.magic_point;
	}
	printf("Magical attack test end. In %d physical attacks, there're %d misses and %d critical attack.\n",
			magical, miss, critical);

}

int test_setup() {
	test_game_logic();
	return 0;
}
