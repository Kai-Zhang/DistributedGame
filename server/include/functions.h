#ifndef __SERVER_H__
#define __SERVER_H__
#include "protocol.h"

int recv_packet(int, char *);
int send_packet(int, struct game_packet *);
int send_to(char *, struct game_packet *);
int broadcast(struct game_packet *);
void login(char *, int);
void logout(char *, int);
void chat_to(char *, char *);
int get_user_sock(char *);
int get_user_status(char *);
bool set_user_status(char *, int);

#endif
