#ifndef __CHARACTER_H__
#define __CHARACTER_H__
#include <cstdlib>

// A player is described by a 5-tuple
struct player {
	int character;		// Specify the character of the player
	int health_point;	// Max value depends on character
	int magic_point;	// Max value depends on character
	int defense;		// Max value depends on character
	int strength;		// Max value depends on character
	int speed;			// Range from 0 to 100
};

// Define the property value
const int PRO_WARRIOR	= 0x1;
const int PRO_MAGICIAN	= 0x2;
const int PRO_ARCHER	= 0x3;

// When player launches a magical attack, the armour will not longer protect
// the defender and the defense point will be ignored.
// Also, when player launches a few attacks, there's a chance the magic
// point will recover a little bit.
const int WARRIOR_MAX_HP		= 5000;
const int WARRIOR_MAX_MP		= 1000;
const int WARRIOR_DEFENSE		= 200;
const int WARRIOR_STRENGTH		= 800;
const int WARRIOR_SPEED			= 60;
const int WARRIOR_MPCONSUME		= 500;
const int WARRIOR_MPRECOVER		= 200;

const int MAGICIAN_MAX_HP		= 2500;
const int MAGICIAN_MAX_MP		= 1500;
const int MAGICIAN_DEFENSE		= 100;
const int MAGICIAN_STRENGTH		= 500;
const int MAGICIAN_SPEED		= 50;
const int MAGICIAN_MPCONSUME	= 200;
const int MAGICIAN_MPRECOVER	= 100;

const int ARCHER_MAX_HP			= 3500;
const int ARCHER_MAX_MP			= 1200;
const int ARCHER_DEFENSE		= 100;
const int ARCHER_STRENGTH		= 1200;
const int ARCHER_SPEED			= 30;
const int ARCHER_MPCONSUME		= 300;
const int ARCHER_MPRECOVER		= 100;

// Define the random event

// When player launches an attack, there's a chance the defender will avoid
// all the damage. The possiblity of attack miss depends on the speed.
static inline bool miss(player& attacker, player& defender) {
	int miss_point = (defender.speed - attacker.speed) / 100;
	return (rand() / RAND_MAX) < miss_point;
}

// When player launches an attack, there's a chance the defender will suffer
// double damage(1.5 damage for magical attack).
static inline bool criticalhit(player& attacker, player& defender) {
	return !((rand() % 10) || (rand() % (attacker.strength / defender.defense)));
}

#endif
