#include <libio.h>
#include <sys/socket.h>
#include <list>
#include <map>
#include <string>
#include <cstring>
using namespace std;

#include "functions.h"

// TODO: please consider the thread safety.
static map<string, int> sockmap;
static list<string> userlist;
int last_list_size = 0;

int get_user_sock(char *username) {
	return sockmap[username];
}

void broadcast(struct game_packet *packet) {
	list<string>::iterator iter = userlist.begin();
	if (packet->service == SERVICE_NAMELIST) {
		packet->pkt_len = 0;
		for (iter = userlist.begin(); iter != userlist.end(); iter ++) {
			strncpy((char *)packet->data + packet->pkt_len, iter->c_str(), NAME_SIZE);
			packet->pkt_len += NAME_SIZE;
		}
	}
	for (iter = userlist.begin(); iter != userlist.end(); iter ++) {
		send_packet(sockmap[(*iter)], packet);
	}
}

int send_packet(int sockfd, struct game_packet *packet) {
	int len = sizeof(struct game_pkt_header) + packet->pkt_len;
	return send(sockfd, packet, len, 0);
}

void send_to(char *username, struct game_packet *packet) {
	send_packet(sockmap[username], packet);
}

void login(char *username, int fd) {
	userlist.push_back(username);
	sockmap.insert(pair<string, int>(username, fd));
	struct game_packet packet;
	packet.service = SERVICE_NAMELIST;
	send_to(NULL, &packet);
}

void logout(char *username, int fd) {
	sockmap.erase(username);
	userlist.remove(username);
	struct game_packet packet;
	packet.service = SERVICE_NAMELIST;
	broadcast(&packet);
}

void chat_to(char *username, char *message) {
	struct game_packet packet;
	packet.service = SERVICE_CHAT;
	strncpy((char *)packet.data, message, sizeof(packet.data));
	broadcast(&packet);
}

