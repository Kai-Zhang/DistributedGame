#include <cstdio>
#include <unistd.h>

#include "protocol.h"
#include "functions.h"

void client_handle(int sockfd) {
	char buffer[sizeof(struct game_packet)];
	int len = 0;
	struct game_packet *packet = (struct game_packet *)buffer;

	if ((len = recv_packet(sockfd, buffer)) <= 0) {
		close(sockfd);
		printf("Socket #%d closed.\n", sockfd);
		return;
	}

	// Authentication accepted
	if (packet->service == SERVICE_LOGIN) {
	}

	// TODO: Add more handler

	close(sockfd);
	printf("Socket #%d closed.\n", sockfd);
}
