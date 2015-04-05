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

	struct game_packet packet;
	packet.service = SERVICE_LOGIN;
	packet.pkt_len = NAME_SIZE;
	strncpy(packet.data, "John", NAME_SIZE);
	send(sockfd, &packet, sizeof(game_pkt_header) + packet.pkt_len, 0);
	assert(recv(sockfd, &packet, sizeof(packet), 0));
	assert(packet.magic_number == 0x55aa);
	assert(packet.service == SERVICE_NAMELIST);
	assert(packet.pkt_len == NAME_SIZE);
	assert(!strcmp(packet.data, "John"));
	puts("Login test approved.");

	packet.magic_number = 0x55aa;
	send(sockfd, &packet, sizeof(game_pkt_header) + packet.pkt_len, 0);
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
	assert(recv(sockfd, &packet, sizeof(packet), 0));
	assert(packet.magic_number == 0x55aa);
	assert(packet.service == SERVICE_CHAT);
	assert(packet.pkt_len == strlen(chat_msg));
	assert(!strcmp(packet.data, chat_msg));
	puts("Chat test approved.");

	close(sockfd);
	return 0;
}
