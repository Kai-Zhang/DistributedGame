#ifndef __CHARACTER_H__
#define __CHARACTER_H__
#include <stdlib.h>



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



#endif
