#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

	// Connection is successfully built. Main loop here.
	// TODO: Main loop
	
	close(sockfd);
	return 0;
}