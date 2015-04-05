#include <cstdio>
#include <cassert>
#include <cstring>
#include "game.h"
#include "protocol.h"
#include "user.h"

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

void test_game_packet() {
	game_packet packet;
	packet.service = SERVICE_GAMEOP;
	packet.pkt_len = sizeof(uint16_t) + 2 * sizeof(player);
	gameop(&packet) = GAME_PHYSICAL_ATTACK;

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

	memcpy(packet.data + sizeof(uint16_t), &p1, sizeof(player));
	memcpy(packet.data + sizeof(uint16_t) + sizeof(player), &p2, sizeof(player));

	assert(*(uint16_t *)packet.data == GAME_PHYSICAL_ATTACK);
	assert(packet.pkt_len == sizeof(uint16_t) + 2 * sizeof(player));
	assert(gameop(&packet) == GAME_PHYSICAL_ATTACK);
	puts("Macro `gameop' is approved.");

	assert((self_player(&packet))->character == p1.character);
	assert((self_player(&packet))->health_point == p1.health_point);
	assert((self_player(&packet))->magic_point == p1.magic_point);
	assert((self_player(&packet))->defense == p1.defense);
	assert((self_player(&packet))->strength == p1.strength);
	assert((self_player(&packet))->speed == p1.speed);
	puts("Macro `self_player' is approved.");

	assert((rival_player(&packet))->character == p2.character);
	assert((rival_player(&packet))->health_point == p2.health_point);
	assert((rival_player(&packet))->magic_point == p2.magic_point);
	assert((rival_player(&packet))->defense == p2.defense);
	assert((rival_player(&packet))->strength == p2.strength);
	assert((rival_player(&packet))->speed == p2.speed);
	puts("Macro `rival_player' is approved.");
}

int test_setup() {
	test_game_logic();
	test_game_packet();
	return 0;
}
