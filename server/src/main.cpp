#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

void *client_handle(void *);
int test_setup();

extern pthread_mutex_t user_mutex;

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
	const int on = 1;
	setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	if ((serverfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Server socket open failed!\n");
		return 1;
	}
	if (bind(serverfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
		perror("Bind socket failed!\n");
		return 1;
	}

	puts("Server is running!");
	pthread_mutex_init(&user_mutex, NULL);
	socklen_t sin_size = sizeof(struct sockaddr_in);
	while (1) {
		listen(serverfd, 10);

		if ((clientfd = accept(serverfd, (struct sockaddr *)&remote_addr, &sin_size)) < 0) {
			perror("Error happens during building connection to server!\n");
			continue;
		}

		printf("Client %s accepted. Socket #%d built.\n", inet_ntoa(remote_addr.sin_addr), clientfd);

		pthread_t handler;
		pthread_create(&handler, NULL, client_handle, (void *)&clientfd);
	}
	pthread_mutex_destroy(&user_mutex);
	close(serverfd);
	return 0;
}
