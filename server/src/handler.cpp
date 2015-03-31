#include <cstdio>
#include <cstring>
#include <unistd.h>

#include "protocol.h"
#include "functions.h"
#include "game.h"

void game_handle(struct game_packet *, int, int&);

struct game_packet failure_packet;

void client_handle(int sockfd) {
	char buffer[sizeof(struct game_packet)];
	struct game_packet *packet = (struct game_packet *)buffer;
	int rivalsock = -1;
	failure_packet.service = SERVICE_FAILED;
	failure_packet.pkt_len = 0;

	if (recv_packet(sockfd, buffer) <= 0) {
		close(sockfd);
		printf("Socket #%d closed.\n", sockfd);
		return;
	}

	if (packet->service == SERVICE_LOGIN) {
		login(packet->data, sockfd);
	}

	while (true) {
		if (recv_packet(sockfd, buffer) <= 0) {
			break;
		}

		char *user = packet->data;
		switch (packet->service) {
			case SERVICE_CHAT:
				while(*user ++);
				if (user - packet->data == packet->pkt_len) {
					broadcast(packet);
				}
				for (int i = 0; i < packet->pkt_len; i += NAME_SIZE) {
					send_to(user, packet);
				}
				break;
			case SERVICE_GAMEREQUEST:
				send_to(packet->data, packet);
				break;
			case SERVICE_GAMEREFUSE:
				send_to(packet->data, packet);
				break;
			case SERVICE_GAMEON:
				rivalsock = get_user_sock(packet->data);
				send_packet(sockfd, packet);
				break;
			case SERVICE_GAMEOP:
				if (rivalsock == -1) {
					send_packet(sockfd, &failure_packet);
				} else if (gameop(packet) == GAME_CHR_CREATE) {
					send_packet(rivalsock, packet);
				} else {
					game_handle(packet, sockfd, rivalsock);
				}
				break;
			default:
				send_packet(sockfd, &failure_packet);
		}
	}

	close(sockfd);
	printf("Socket #%d closed.\n", sockfd);
}

void game_handle(struct game_packet *packet, int selfsock, int &rivalsock) {
	struct player *self = self_player(packet);
	struct player *rival = rival_player(packet);
	struct game_packet return_packet;
	int attack_result = 0;
	switch (gameop(packet)) {
		case GAME_PHYSICAL_ATTACK:
			attack_result = physical_attack(*self, *rival);
			gameop(packet) |= attack_result;
			send_packet(selfsock, packet);
			if (attack_result & EFFECT_SETTLED) {
				memcpy(&return_packet, packet, sizeof(struct game_packet));
				gameop(&return_packet) |= GAME_LOSE;
				send_packet(rivalsock, &return_packet);
				rivalsock = -1;
			}
			break;
		case GAME_MAGICAL_ATTACK:
			attack_result = magical_attack(*self, *rival);
			gameop(packet) |= attack_result;
			send_packet(selfsock, packet);
			if (attack_result & EFFECT_SETTLED) {
				memcpy(&return_packet, packet, sizeof(struct game_packet));
				gameop(&return_packet) |= GAME_LOSE;
				send_packet(rivalsock, &return_packet);
				rivalsock = -1;
			}
			break;
		case GAME_RETREAT:
			send_packet(rivalsock, packet);
			rivalsock = -1;
			break;
		case GAME_CONCEDE:
			gameop(packet) |= GAME_WIN;
			send_packet(rivalsock, packet);
			rivalsock = -1;
			break;
		default:
			send_packet(selfsock, &failure_packet);
	}
}
