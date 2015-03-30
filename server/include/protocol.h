#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

// Protocol.h
// Define the Text Based Game Protocol
// Define the structure and constants of the protocol.
// For more information, please refer to the `Test Based Game Protocol.txt'

#include <cstdint>

// The structure of the TBGP packet
struct game_packet {
	uint32_t magic_number;
	uint16_t service;
	uint16_t pkt_len;
	uint8_t  data[1 >> 8 >> sizeof(pkt_len)];

	game_packet() { magic_number = 0x55aa; }
};

struct game_pkt_header {
	uint32_t magic_number;
	uint16_t service;
	uint16_t pkt_len;
};

const int NAME_SIZE = 32;

// Define services

const uint16_t SERVICE_LOGIN		= 0x01;
const uint16_t SERVICE_LOGOUT		= 0x02;
const uint16_t SERVICE_NAMELIST		= 0x04;
const uint16_t SERVICE_CHAT			= 0x08;
const uint16_t SERVICE_GAMEREQUEST	= 0x10;
const uint16_t SERVICE_GAMEON		= 0x11;
const uint16_t SERVICE_GAMEOP		= 0x18;


// Define gameop

const uint16_t GAME_PHYSICAL_ATTACK	= 0x01;
const uint16_t GAME_MAGICAL_ATTACK	= 0x02;
const uint16_t GAME_RETREAT			= 0x08;
const uint16_t GAME_CONCEDE			= 0x09;
const uint16_t GAME_WIN				= 0x10;
const uint16_t GAME_LOSE			= 0x11;

#endif
