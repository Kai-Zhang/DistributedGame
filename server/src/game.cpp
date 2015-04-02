#include "game.h"

const int mp_consume[3] = {
	WARRIOR_MPCONSUME,
	MAGICIAN_MPCONSUME,
	ARCHER_MPCONSUME
};

int physical_attack(player& attacker, player& defender) {
	if (miss(attacker, defender)) {
		return EFFECT_MISS;
	}
	int attack_status = EFFECT_NOTHING;

	defender.health_point -= (attacker.strength - defender.defense);
	if (criticalhit(attacker, defender)) {
		attack_status |= EFFECT_CRITICAL_HIT;
		defender.health_point -= (attacker.strength - defender.defense);
	}

	if (defender.health_point > 0) {
		return attack_status;
	}
	return attack_status | EFFECT_SETTLED;
}

int magical_attack(player& attacker, player& defender) {
	if (miss(attacker, defender)) {
		return EFFECT_MISS;
	}
	int attack_status = EFFECT_NOTHING;

	attacker.magic_point -= mp_consume[attacker.character];
	defender.health_point -= attacker.strength;
	if (criticalhit(attacker, defender)) {
		attack_status |= EFFECT_CRITICAL_HIT;
		defender.health_point -= (attacker.strength / 2);
	}

	if (defender.health_point > 0) {
		return attack_status;
	}
	return attack_status | EFFECT_SETTLED;
}

