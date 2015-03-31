#ifndef __GAME_H__
#define __GAME_H__
#include "character.h"

#define gameop(x) (*(uint16_t *)((x)->data))
#define self_player(x) (struct player *)((x)->data + sizeof(uint16_t))
#define rival_player(x) (struct player *)((x)->data + 2 * sizeof(uint16_t))

const int EFFECT_NOTHING        = 0x00;
const int EFFECT_SETTLED        = 0x80;
const int EFFECT_MISS           = 0x04;
const int EFFECT_CRITICAL_HIT   = 0x08;

int physical_attack(player&, player&);
int magical_attack(player&, player&);

#endif
