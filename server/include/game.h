#ifndef __GAME_H__
#define __GAME_H__
#include "character.h"

const int EFFECT_NOTHING = 0;
const int EFFECT_SETTLED = 1;

int physical_attack(player&, player&);
int magical_attack(player&, player&);

#endif
