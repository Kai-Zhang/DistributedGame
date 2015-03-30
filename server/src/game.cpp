#include "game.h"

const int mp_consume[3] = {
	WARRIOR_MPCONSUME,
	MAGICIAN_MPCONSUME,
	ARCHER_MPCONSUME
};

int physical_attack(player& attacker, player& defender) {
	if (miss(attacker, defender)) {
		return EFFECT_NOTHING;
	}

	defender.health_point -= (attacker.strength - defender.defense);
	if (criticalhit(attacker, defender)) {
		defender.health_point -= (attacker.strength - defender.defense);
	}

	if (defender.health_point > 0) {
		return EFFECT_NOTHING;
	}
	return EFFECT_SETTLED;
}

int magical_attack(player& attacker, player& defender) {
	if (miss(attacker, defender)) {
		return EFFECT_NOTHING;
	}

	attacker.magic_point -= mp_consume[attacker.character];
	defender.health_point -= attacker.strength;
	if (criticalhit(attacker, defender)) {
		defender.health_point -= (attacker.strength / 2);
	}

	if (defender.health_point > 0) {
		return EFFECT_NOTHING;
	}
	return EFFECT_SETTLED;
}

