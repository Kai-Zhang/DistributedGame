#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "protocol.h"
#include "game.h"

int init_sock(char **sys_args) {
	int sockfd = -1;
	struct sockaddr_in remote_addr;
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(sys_args[1]);
	remote_addr.sin_port = htons(atoi(sys_args[2]));
	
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
	if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0) {
		return -1;
	}
	return sockfd;
}


int main(int argc, char *argv[]) {
	if (argc != 3) {
		puts("Usage: client [server ip] [server port]");
		return -1;
	}

	int sockfd = -1;
	if ((sockfd = init_sock(argv)) < 0 ) {
		perror("Can not connect to server!\n");
		return 1;
	}

	const char *myname = "John";
	struct game_packet packet;
	packet.service = SERVICE_LOGIN;
	packet.pkt_len = NAME_SIZE;
	strncpy(packet.data, myname, NAME_SIZE);
	send(sockfd, &packet, sizeof(game_pkt_header) + packet.pkt_len, 0);
	memset(&packet, 0, sizeof(packet));
	assert(recv(sockfd, &packet, sizeof(packet), 0));
	assert(packet.magic_number == 0x55aa);
	assert(packet.service == SERVICE_NAMELIST);
	assert(packet.pkt_len == NAME_SIZE);
	assert(!strcmp(packet.data, myname));
	puts("Login test approved.");

	send(sockfd, &packet, sizeof(game_pkt_header) + packet.pkt_len, 0);
	memset(&packet, 0, sizeof(packet));
	assert(recv(sockfd, &packet, sizeof(packet), 0));
	assert(packet.magic_number == 0x55aa);
	assert(packet.service == SERVICE_FAILED);
	assert(packet.pkt_len == 0);
	puts("Failure message test approved.");

	const char *chat_msg = "Hello, world!";
	packet.service = SERVICE_CHAT;
	packet.pkt_len = strlen(chat_msg);
	strncpy(packet.data, chat_msg, NAME_SIZE);
	send(sockfd, &packet, sizeof(game_pkt_header) + packet.pkt_len, 0);
	memset(&packet, 0, sizeof(packet));
	assert(recv(sockfd, &packet, sizeof(packet), 0));
	assert(packet.magic_number == 0x55aa);
	assert(packet.service == SERVICE_CHAT);
	assert(packet.pkt_len == strlen(chat_msg));
	assert(!strcmp(packet.data, chat_msg));
	puts("Chat test approved.");

	packet.service = SERVICE_GAMEREQUEST;
	packet.pkt_len = 2 * NAME_SIZE;
	strncpy(packet.data, myname, NAME_SIZE);
	strncpy(packet.data + NAME_SIZE, myname, NAME_SIZE);
	send(sockfd, &packet, sizeof(game_pkt_header) + packet.pkt_len, 0);
	memset(&packet, 0, sizeof(packet));
	assert(recv(sockfd, &packet, sizeof(packet), 0));
	assert(packet.magic_number == 0x55aa);
	assert(packet.service == SERVICE_GAMEREQUEST);
	assert(packet.pkt_len == 2 * NAME_SIZE);
	assert(!strcmp(packet.data, myname));
	assert(!strcmp(packet.data + NAME_SIZE, myname));
	puts("Game request test approved.");

	packet.service = SERVICE_GAMEON;
	packet.pkt_len = 2 * NAME_SIZE;
	strncpy(packet.data, myname, NAME_SIZE);
	strncpy(packet.data + NAME_SIZE, myname, NAME_SIZE);
	send(sockfd, &packet, sizeof(game_pkt_header) + packet.pkt_len, 0);
	memset(&packet, 0, sizeof(packet));
	assert(recv(sockfd, &packet, sizeof(packet), 0));
	assert(packet.magic_number == 0x55aa);
	assert(packet.service == SERVICE_GAMEON);
	assert(packet.pkt_len == 2 * NAME_SIZE);
	assert(!strcmp(packet.data, myname));
	assert(!strcmp(packet.data + NAME_SIZE, myname));
	puts("Game on now.");

	player p;
	p.character = PRO_WARRIOR;
	p.health_point = WARRIOR_MAX_HP;
	p.magic_point = WARRIOR_MAX_MP;
	p.defense = WARRIOR_DEFENSE;
	p.strength = WARRIOR_STRENGTH;
	p.speed = WARRIOR_SPEED;

	packet.service = SERVICE_GAMEOP;
	packet.pkt_len = sizeof(uint16_t) + sizeof(player);
	gameop(&packet) = GAME_CHR_CREATE;
	memcpy(self_player(&packet), &p, sizeof(player));
	send(sockfd, &packet, sizeof(game_pkt_header) + packet.pkt_len, 0);
	memset(&packet, 0, sizeof(packet));
	assert(recv(sockfd, &packet, sizeof(packet), 0));
	assert(packet.magic_number == 0x55aa);
	assert(packet.service == SERVICE_GAMEOP);
	assert(gameop(&packet) == GAME_CHR_CREATE);
	assert(packet.pkt_len == sizeof(uint16_t) + sizeof(player));
	assert(!memcmp(self_player(&packet), &p, sizeof(player)));
	puts("Character successfully create.");

	packet.service = SERVICE_GAMEOP;
	packet.pkt_len = sizeof(uint16_t) + sizeof(player);
	gameop(&packet) = GAME_CHR_CREATE;
	memcpy(self_player(&packet), &p, sizeof(player));
	send(sockfd, &packet, sizeof(game_pkt_header) + packet.pkt_len, 0);
	memset(&packet, 0, sizeof(packet));
	assert(recv(sockfd, &packet, sizeof(packet), 0));
	assert(packet.magic_number == 0x55aa);
	assert(packet.service == SERVICE_GAMEOP);
	assert(gameop(&packet) == GAME_CHR_CREATE);
	assert(packet.pkt_len == sizeof(uint16_t) + sizeof(player));
	assert(!memcmp(self_player(&packet), &p, sizeof(player)));
	puts("Rival's character successfully create.");

	packet.service = SERVICE_GAMEOP;
	packet.pkt_len = sizeof(uint16_t) + 2 * sizeof(player);
	gameop(&packet) = GAME_PHYSICAL_ATTACK;
	memcpy(self_player(&packet), &p, sizeof(player));
	memcpy(rival_player(&packet), &p, sizeof(player));
	send(sockfd, &packet, sizeof(game_pkt_header) + packet.pkt_len, 0);
	memset(&packet, 0, sizeof(packet));
	assert(recv(sockfd, &packet, sizeof(packet), 0));
	assert(packet.magic_number == 0x55aa);
	assert(packet.service == SERVICE_GAMEOP);
	assert(packet.pkt_len == sizeof(uint16_t) + 2 * sizeof(player));
	assert(!memcmp(self_player(&packet), &p, sizeof(player)));
	// Since game logic is tested. The test will not calculate again.
	puts("Game is going well.");

	packet.service = SERVICE_GAMEOP;
	packet.pkt_len = sizeof(uint16_t) + 2 * sizeof(player);
	gameop(&packet) = GAME_CONCEDE;
	memcpy(self_player(&packet), &p, sizeof(player));
	memcpy(rival_player(&packet), &p, sizeof(player));
	send(sockfd, &packet, sizeof(game_pkt_header) + packet.pkt_len, 0);
	memset(&packet, 0, sizeof(packet));
	assert(recv(sockfd, &packet, sizeof(packet), 0));
	assert(packet.magic_number == 0x55aa);
	assert(packet.service == SERVICE_GAMEOP);
	assert(gameop(&packet) & GAME_WIN);
	assert(packet.pkt_len == sizeof(uint16_t) + 2 * sizeof(player));
	assert(!memcmp(self_player(&packet), &p, sizeof(player)));
	puts("Concede okay.");

	puts("All tests approved.");

	close(sockfd);
	return 0;
}
