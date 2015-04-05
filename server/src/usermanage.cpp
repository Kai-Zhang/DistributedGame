#include <libio.h>
#include <sys/socket.h>
#include <list>
#include <map>
#include <string>
#include <cstring>
using namespace std;

#include "functions.h"
#include "user.h"

// TODO: please consider the thread safety.
static map<string, int> sockmap;
static list<struct online_user> userlist;
int last_list_size = 0;

int get_user_sock(char *username) {
	return sockmap[username];
}

int get_user_status(char *username) {
	for (list<struct online_user>::iterator iter = userlist.begin();
			iter != userlist.end(); iter++) {
		if (iter->username == username) {
			return iter->status;
		}
	}
	return -1;
}

bool set_user_status(char *username, int status) {
	for (list<struct online_user>::iterator iter = userlist.begin();
			iter != userlist.end(); iter++) {
		if (iter->username == username) {
			iter->status = status;
			return true;
		}
	}
	return false;
}

void broadcast(struct game_packet *packet) {
	list<struct online_user>::iterator iter = userlist.begin();
	if (packet->service == SERVICE_NAMELIST) {
		packet->pkt_len = 0;
		for (iter = userlist.begin(); iter != userlist.end(); iter ++) {
			strncpy((char *)packet->data + packet->pkt_len, iter->username.c_str(), NAME_SIZE);
			packet->pkt_len += NAME_SIZE;
		}
	}
	for (iter = userlist.begin(); iter != userlist.end(); iter ++) {
		send_packet(sockmap[(iter->username)], packet);
	}
}

int send_packet(int sockfd, struct game_packet *packet) {
	int len = sizeof(struct game_pkt_header) + packet->pkt_len;
	return send(sockfd, packet, len, 0);
}

void send_to(char *username, struct game_packet *packet) {
	if (username == NULL) {
		broadcast(packet);
	}
	else {
		send_packet(sockmap[username], packet);
	}
}

void login(char *username, int fd) {
	userlist.push_back({username, USER_AVAILABLE});
	sockmap.insert(pair<string, int>(username, fd));
	struct game_packet packet;
	packet.service = SERVICE_NAMELIST;
	int count = 0;
	for (list<struct online_user>::iterator iter = userlist.begin();
			iter != userlist.end(); iter++) {
		strncpy(packet.data + count * 32, iter->username.c_str(), NAME_SIZE);
		count ++;
	}
	packet.pkt_len = count * 32;
	send_to(NULL, &packet);
}

void logout(char *username, int fd) {
	sockmap.erase(username);
	userlist.remove({username, 0});
	struct game_packet packet;
	packet.service = SERVICE_NAMELIST;
	int count = 0;
	for (list<struct online_user>::iterator iter = userlist.begin();
			iter != userlist.end(); iter++) {
		strncpy(packet.data + count * 32, iter->username.c_str(), NAME_SIZE);
		count ++;
	}
	packet.pkt_len = count * 32;
	broadcast(&packet);
}

void chat_to(char *username, char *message) {
	struct game_packet packet;
	packet.service = SERVICE_CHAT;
	strncpy((char *)packet.data, message, sizeof(packet.data));
	broadcast(&packet);
}

