#include <sys/socket.h>

#include "protocol.h"

int n_recv(int, char *, size_t);

int recv_packet(int sockfd, char *buffer) {
	int rc = 0;

	if ((rc = recv(sockfd, buffer, sizeof(game_pkt_header), 0)) != sizeof(game_pkt_header)) {
		return rc < 0 ? -1 : 0;
	}
	if (((struct game_pkt_header *)buffer)->magic_number != 0x55aa) {
		return -1;
	}

	int len = ((struct game_pkt_header *)buffer)->pkt_len;
	
	rc = n_recv(sockfd, buffer + sizeof(struct game_pkt_header), len);
	if (rc != len) {
		return rc < 0? -1 : 0;
	}

	return rc;
}

int n_recv(int sockfd, char *buffer, size_t len) {
	int cnt = len;
	int rc = 0;

	while (cnt > 0) {
		rc = recv(sockfd, buffer, cnt, 0);
		if (rc < 0) {
			return -1;
		}
		if (rc == 0) {
			return len - cnt;
		}
		buffer += rc;
		cnt -= rc;
	}
	return len;
}
