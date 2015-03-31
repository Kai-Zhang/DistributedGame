#ifndef __SERVER_H__
#define __SERVER_H__
#include "protocol.h"

int recv_packet(int, char *);
int send_packet(int, struct game_packet *);
void send_to(char *, struct game_packet *);
void login(char *, int);
void logout(char *, int);
void chat_to(char *, char *);

#endif