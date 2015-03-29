#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <pthread.h>

void *client_handle(void *sockfd_ptr) {
	int sockfd = *(int *)sockfd_ptr;

	// TODO: Function here!

	printf("Socket #%d closed.\n", sockfd);
	pthread_exit((void *)0);
}
