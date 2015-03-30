#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

void client_handle(int);

int main(int argc, char *argv[]) {
	if (argc != 2) {
		puts("Usage: server [port]");
		return -1;
	}
	int serverfd = -1, clientfd = -1;
	struct sockaddr_in server_addr, remote_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(atoi(argv[1]));

	if ((serverfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Server socket open failed!\n");
		return 1;
	}
	if (bind(serverfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
		perror("Bind socket failed!\n");
		return 1;
	}

	puts("Server is running!");
	socklen_t sin_size = sizeof(struct sockaddr_in);
	while (1) {
		listen(serverfd, 10);

		if ((clientfd = accept(serverfd, (struct sockaddr *)&remote_addr, &sin_size)) < 0) {
			perror("Error happens during building connection to server!\n");
			continue;
		}

		printf("Client %s accepted. Socket #%d built.\n", inet_ntoa(remote_addr.sin_addr), clientfd);

		std::thread handler(client_handle, clientfd);
		handler.detach();
	}
	close(serverfd);
	return 0;
}
