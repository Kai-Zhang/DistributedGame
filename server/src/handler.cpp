#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <pthread.h>

#include "protocol.h"
#include "functions.h"
#include "game.h"
#include "user.h"

void game_handle(struct game_packet *, int);

struct online_user current_user;

struct game_packet failure_packet;
struct game_packet refuse_packet;

void *client_handle(void *sockfd_ptr) {
	int sockfd = *(int *)sockfd_ptr;
	char buffer[sizeof(struct game_packet)];
	struct game_packet *packet = (struct game_packet *)buffer;
	char *current_username = NULL;
	failure_packet.service = SERVICE_FAILED;
	failure_packet.pkt_len = 0;
	refuse_packet.service = SERVICE_GAMEREFUSE;
	refuse_packet.pkt_len = 32;

	while (true) {
		if (recv_packet(sockfd, buffer) <= 0) {
			break;
		}

		char *user = packet->data;
		if (current_username == NULL && packet->service != SERVICE_LOGIN) {
			send_packet(sockfd, &failure_packet);
		}
		switch (packet->service) {
			case SERVICE_CHAT:
				while(*user ++);
				if (user - packet->data == packet->pkt_len + 1) {
					broadcast(packet);
				}
				for (int i = 0; i < packet->pkt_len; i += NAME_SIZE) {
					send_to(user, packet);
				}
				break;
			case SERVICE_GAMEREQUEST:
				if (get_user_status(current_username) == USER_AVAILABLE) {
					send_to(packet->data, packet);
					set_user_status(current_username, USER_BUSY);
				} else {
					strncpy(refuse_packet.data, current_username, NAME_SIZE);
					send_packet(sockfd, &refuse_packet);
				}
				break;
			case SERVICE_GAMEREFUSE:
				send_to(packet->data, packet);
				set_user_status(current_username, USER_AVAILABLE);
				break;
			case SERVICE_GAMEON:
				current_user.rivalsock = get_user_sock(packet->data);
				send_to(packet->data, packet);
				break;
			case SERVICE_GAMEOP:printf("111");
				if (current_user.rivalsock == -1) {
					send_packet(sockfd, &failure_packet);
				} else if (gameop(packet) == GAME_CHR_CREATE) {
					send_packet(current_user.rivalsock, packet);
				} else {
					game_handle(packet, sockfd);
				}
				break;
			case SERVICE_LOGOUT:
				logout(current_username, sockfd);
				current_username = NULL;
			case SERVICE_LOGIN:
				current_user.username = packet->data;
				current_username = (char *)current_user.username.c_str();
				login(packet->data, sockfd);
			default:
				send_packet(sockfd, &failure_packet);
		}
	}

	if (current_username) {
		logout(current_username, sockfd);
	}
	close(sockfd);
	printf("Socket #%d closed.\n", sockfd);
	pthread_exit((void *)0);
}

void game_handle(struct game_packet *packet, int selfsock) {
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
				send_packet(current_user.rivalsock, &return_packet);
				current_user.status = USER_AVAILABLE;
				current_user.rivalsock = -1;
			}
			break;
		case GAME_MAGICAL_ATTACK:
			attack_result = magical_attack(*self, *rival);
			gameop(packet) |= attack_result;
			send_packet(selfsock, packet);
			if (attack_result & EFFECT_SETTLED) {
				memcpy(&return_packet, packet, sizeof(struct game_packet));
				gameop(&return_packet) |= GAME_LOSE;
				send_packet(current_user.rivalsock, &return_packet);
				current_user.status = USER_AVAILABLE;
				current_user.rivalsock = -1;
			}
			break;
		case GAME_RETREAT:
			send_packet(current_user.rivalsock, packet);
			current_user.status = USER_AVAILABLE;
			current_user.rivalsock = -1;
			break;
		case GAME_CONCEDE:
			gameop(packet) |= GAME_WIN;
			send_packet(current_user.rivalsock, packet);
			current_user.status = USER_AVAILABLE;
			current_user.rivalsock = -1;
			break;
		default:
			send_packet(selfsock, &failure_packet);
	}
}
