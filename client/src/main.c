
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <protocol.h>
#include <character.h>

#define MAXLINE 128

static int login = 0;
//important tip : not decided yet will also make Gameon 1;
static int Gameon = 0;
static int Chatout = 0;



char selfname[16];
int setfor_for = 0;
int lenofdata = 0;
char sendline[MAXLINE],sendline1[MAXLINE],sendline2[MAXLINE],recvline1[MAXLINE],recvline2[MAXLINE],recvline3[MAXLINE],recvdata[MAXLINE],senddata[MAXLINE];
struct player local,server;


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

void struct_player_print(struct player a){
    switch (a.character) {
        case 0x1 :printf("character : warrior\n");break;
        case 0x2 :printf("character : magician\n");break;
        case 0x3 :printf("character : archer\n");break;
    }
    printf("health point : %d\n",a.health_point);
    printf("magicpoint : %d\n",a.magic_point);
    printf("defense : %d\n",a.defense);
    printf("strength : %d\n",a.strength);
    printf("speed : %d\n",a.speed);
}

void struct_gameop_print(struct game_op_packet *gameop) {
    switch (gameop->game_op) {
		case 0x01: {
		    printf("physical attack !\nFrom:\n");
		    struct_player_print(gameop->from);
		    printf("to:\n");
		    struct_player_print(gameop->to);
		    break;
		    };
		case 0x02: {
		    printf("magical attack !\nFrom:\n");
		    struct_player_print(gameop->from);
		    printf("to:\n");
		    struct_player_print(gameop->to);
		    break;
		    };
		case 0x04: {
		    printf("attact miss !\nFrom:\n");
		    struct_player_print(gameop->from);
		    printf("to:\n");
		    struct_player_print(gameop->to);
		    break;
		    };
		case 0x08: {
		    printf("physical attack !\nFrom:\n");
		    struct_player_print(gameop->from);
		    printf("to:\n");
		    struct_player_print(gameop->to);
		    break;
		    };
		case 0x10: {
		    printf("retreat !\nFrom:\n");
		    struct_player_print(gameop->from);
		    printf("to:\n");
		    struct_player_print(gameop->to);
		    break;
		    };
		case 0x11: {
		    printf("concede !\nFrom:\n");
		    struct_player_print(gameop->from);
		    printf("to:\n");
		    struct_player_print(gameop->to);
		    break;
		    };
		case 0x80: {
		    printf("you have won !\n");
		    Gameon = 0;
		    break;
		    };
		case 0xc0: {
		    printf("you have lost !\n");
		    Gameon = 0;
		    break;
		    };
	}
}
 
 
 
            //there should be 4 thread : 
            //1. namelist change and chat from server
            void *get1(void * conn){
	            int sockfd=(int)conn;
	            
                while (recv(sockfd, recvline1, MAXLINE, 0) != 0) {
                    printf("11111\n");
                    struct game_packet *packet = (struct game_packet *)recvline1;
		            if(packet->magic_number == 0x55aa) {
                        if(packet->service == SERVICE_NAMELIST) {
                            while(!Chatout){
                            }
                            printf("new user :%s",packet->data);
                        }
                        //waited to be discussed
                        if(packet->service == SERVICE_CHAT) {
                            lenofdata = packet->pkt_len;
                            for(setfor_for = 0; setfor_for < lenofdata; setfor_for++) {
                                recvdata[setfor_for] = recvline1[setfor_for + 8];
                            }
                            while(!Chatout){
                            }
                            printf("chat :%s",recvdata);
                            memset(recvdata, 0, sizeof(recvdata));
                        }
                    }
                }
            }
            //2. chat and logout from local
            void *get2(void * conn){
                printf("1111\n");
                int sockfd=(int)conn;
                while (fgets(sendline1, MAXLINE, stdin) != NULL) {
                    printf("11\n");
                    if(sendline1[0] == 'c') {
                        Chatout = 1;
                        struct game_packet *packet = (struct game_packet *)sendline1;
                        packet->magic_number = 0x55aa;
                        packet->service = SERVICE_CHAT;
                        packet->pkt_len = strlen(sendline1) - 1;
                        for(setfor_for = 0; setfor_for < packet->pkt_len; setfor_for++) {
                            packet->data[setfor_for] = sendline1[setfor_for + 1];
                        }
                        send(sockfd, packet, packet->pkt_len + 8, 0);
                        Chatout = 0;
                    }
                        
                    if(sendline1[0] == 'C') {
                        Chatout = 1;
                        struct game_packet *packet = (struct game_packet *)sendline1;
                        packet->magic_number = 0x55aa;
                        packet->service = SERVICE_CHAT;
                        packet->pkt_len = strlen(sendline) - 1;
                        for(setfor_for = 0; setfor_for < packet->pkt_len; setfor_for++) {
                            if(sendline[setfor_for + 1] == '#') {
                                packet->data[setfor_for] = '\0';
                            }
                            else {
                                packet->data[setfor_for] = sendline[setfor_for + 1];
                            }
                        }
                        send(sockfd, packet, packet->pkt_len + 8, 0);
                        Chatout = 0;
                    }
                    if(sendline[0] == 'L') {
                        struct game_packet *packet = (struct game_packet *)sendline1;
                        packet->magic_number = 0x55aa;
                        packet->service = SERVICE_LOGIN;
                        packet->pkt_len = strlen(sendline)-1;
                        for(setfor_for = 0; setfor_for < packet->pkt_len; setfor_for++) {
                            packet->data[setfor_for] = sendline[setfor_for + 1];
                            selfname[setfor_for] = sendline[setfor_for + 1];
                        }
                        send(sockfd, packet, packet->pkt_len + 8, 0);
                        printf("you have logged out\n");
                        login = 0;
                        printf("Please log in(started with '#' or the name will not be accepted)");

                        pthread_exit((void *)0);
                        break;
                    }   
                }
           }
           
           
           //3. request from local and then game
           void *get3(void * conn){
                int sockfd=(int)conn;
                while(fgets(sendline2, MAXLINE, stdin) != NULL) {
                    if(sendline2[0] == 'R') {
                        if(Gameon == 0) {
                            struct game_start_packet *packet = (struct game_packet *)sendline2;
                            Gameon = 1;
                            packet->magic_number = 0x55aa;
                            packet->service = 0x10;
                            packet->pkt_len = 32;
                            fgets(packet->data1, 16, stdin);
                            strcpy(packet->data2,selfname);
                            send(sockfd, packet, 40, 0);
                            while(recv(sockfd, recvline2, MAXLINE, 0) != 0) {
                                struct game_packet *packet = (struct game_packet *)recvline2;
		                        if((packet->magic_number == 0x55aa) && (packet->service == SERVICE_GAMEON)) {
		                        //game truly on now
		                        //a while loop for the interaction of gameop
		                            memset(recvline2, 0, sizeof(recvline2));
		                            struct game_packet *a;
		                            a->magic_number = 0x55aa;
                                    a->service = SERVICE_GAMEOP;
                                    a->pkt_len = 2;
                                    struct game_create_packet *gcpacket = (struct game_create_packet *)a->data;
                                    gcpacket->game_op = GAME_CHR_CREATE;
                                    printf("you can choose warrior,magician or archer by 'w','m' or 'a'.");
                                    char c = getchar();
                                    if(c == 'w') {
                                        gcpacket->character = PRO_WARRIOR;
                                        local.character = PRO_WARRIOR;
                                        local.health_point = WARRIOR_MAX_HP;
                                        local.magic_point = WARRIOR_MAX_MP;
                                        local.defense = WARRIOR_DEFENSE;
                                        local.strength = WARRIOR_STRENGTH;
                                        local.speed = WARRIOR_SPEED;
                                    }
                                    if(c == 'm') {
                                        gcpacket->character = PRO_MAGICIAN;
                                        local.character = PRO_MAGICIAN;
                                        local.health_point = MAGICIAN_MAX_HP;
                                        local.magic_point = MAGICIAN_MAX_MP;
                                        local.defense = MAGICIAN_DEFENSE;
                                        local.strength = MAGICIAN_STRENGTH;
                                        local.speed = MAGICIAN_SPEED;
                                    }
                                    if(c == 'a') {
                                        gcpacket->character = PRO_ARCHER;
                                        local.character = PRO_ARCHER;
                                        local.health_point = ARCHER_MAX_HP;
                                        local.magic_point = ARCHER_MAX_MP;
                                        local.defense = ARCHER_DEFENSE;
                                        local.strength = ARCHER_STRENGTH;
                                        local.speed = ARCHER_SPEED;
                                    }
                                    send(sockfd, a, 12, 0);
                                    //printf
		                            while(recv(sockfd, recvline2 , MAXLINE, 0) != 0) {
		                                struct game_packet *packet = (struct game_packet *)recvline2;
		                                if((packet->magic_number == 0x55aa) && (packet->service == SERVICE_GAMEOP)){
		                                    struct game_op_packet *gameop = (struct game_op_packet *)packet->data;
		                                    //printf
		                                    struct_gameop_print(gameop);
		                                    //now your turn 
		                                    printf(" you need to decide what you will do now :\n");
		                                    printf("p :physical attack\nm :magical attack\nf : give up\n");
		                                    struct game_packet *a;
		                                    a->magic_number = 0x55aa;
		                                    a->pkt_len = 14;
		                                    a->service = SERVICE_GAMEOP;
		                                    struct game_op_packet *gameop2 = (struct game_op_packet *)a->data;
		                                    char c = getchar();
		                                    switch (c) {
		                                        case 'p':gameop2->game_op = 0x01;
		                                        case 'm':gameop2->game_op = 0x02;
		                                        case 'f':gameop2->game_op = 0xc0;
		                                    }
		                                    gameop2->from = gameop->to;
		                                    gameop2->to = gameop->from;
		                                    send(sockfd, a, 22, 0);
		                                    if(Gameon == 0) {
		                                        break;
		                                    }
		                                }
		                            }
		                        }
		                        if(Gameon == 0) {
		                             break;
		                        }
                            }
                        }
                        if(Gameon == 1) {
                            printf("you are playing and could not launch request");
                        }
                        Gameon = 0;
                    }
                }
           }
           void *get4(void *conn){
	        int sockfd=(int)conn;
                //4. request from server and then game
                while(recv(sockfd, recvline3, MAXLINE, 0) != 0) {
                    if(Gameon == 0){
                        struct game_start_packet *packet = (struct game_packet *)recvline3;
                        if((packet->magic_number == 0x55aa) && (packet->service == SERVICE_GAMEREQUEST)) {
                            //printf
                            printf("if you want to play game with %s please type in a 'y' or you should type in an 'n'",packet->data2);
                            if(getchar() == 'y'){
                                struct game_start_packet b;
                                b.magic_number = 0x55aa;
                                b.service = SERVICE_GAMEON;
                                b.pkt_len = 32;
                                strcpy(b.data1,packet->data2);
                                strcpy(b.data2,packet->data1);
                                send(sockfd, &b, 40, 0);
                                //now game really on
                                struct game_packet *a;
		                        a->magic_number = 0x55aa;
                                a->service = SERVICE_GAMEOP;
                                a->pkt_len = 2;
                                struct game_create_packet *gcpacket = (struct game_create_packet *)a->data;
                                gcpacket->game_op = GAME_CHR_CREATE;
                                printf("you can choose warrior,magician or archer by 'w','m' or 'a'.");
                                char c = getchar();
                                while (1) {
                                    if(c == 'w') {
                                        gcpacket->character = PRO_WARRIOR;
                                        local.character = PRO_WARRIOR;
                                        local.health_point = WARRIOR_MAX_HP;
                                        local.magic_point = WARRIOR_MAX_MP;
                                        local.defense = WARRIOR_DEFENSE;
                                        local.strength = WARRIOR_STRENGTH;
                                        local.speed = WARRIOR_SPEED;
                                        break;
                                    }
                                    if(c == 'm') {
                                        gcpacket->character = PRO_MAGICIAN;
                                        local.character = PRO_MAGICIAN;
                                        local.health_point = MAGICIAN_MAX_HP;
                                        local.magic_point = MAGICIAN_MAX_MP;
                                        local.defense = MAGICIAN_DEFENSE;
                                        local.strength = MAGICIAN_STRENGTH;
                                        local.speed = MAGICIAN_SPEED;
                                        break;
                                    }
                                    if(c == 'a') {
                                        gcpacket->character = PRO_ARCHER;
                                        local.character = PRO_ARCHER;
                                        local.health_point = ARCHER_MAX_HP;
                                        local.magic_point = ARCHER_MAX_MP;
                                        local.defense = ARCHER_DEFENSE;
                                        local.strength = ARCHER_STRENGTH;
                                        local.speed = ARCHER_SPEED;
                                        break;
                                    }
                                }
                                send(sockfd, a, 12, 0);
                                
                                while (1){
                                    recv(sockfd, recvline3, MAXLINE, 0);
                                    struct game_packet *d = (struct game_packet *)recvline3;
                                    if((d->magic_number == 0x55aa) && (d->service == SERVICE_GAMEOP) && (d->pkt_len == 2)) {
                                        struct game_create_packet *e = (struct game_create_packet *)d->data;
                                        if(e->game_op == GAME_CHR_CREATE) {
                                            if(e->character == 0x1) {
                                                server.character = PRO_WARRIOR;
                                                server.health_point = WARRIOR_MAX_HP;
                                                server.magic_point = WARRIOR_MAX_MP;
                                                server.defense = WARRIOR_DEFENSE;
                                                server.strength = WARRIOR_STRENGTH;
                                                server.speed = WARRIOR_SPEED;
                                                break;
                                            }
                                            if(e->character == 0x2) {
                                                server.character = PRO_MAGICIAN;
                                                server.health_point = MAGICIAN_MAX_HP;
                                                server.magic_point = MAGICIAN_MAX_MP;
                                                server.defense = MAGICIAN_DEFENSE;
                                                server.strength = MAGICIAN_STRENGTH;
                                                server.speed = MAGICIAN_SPEED;
                                                break;
                                            }
                                            if(e->character == 0x3) {
                                                server.character = PRO_ARCHER;
                                                server.health_point = ARCHER_MAX_HP;
                                                server.magic_point = ARCHER_MAX_MP;
                                                server.defense = ARCHER_DEFENSE;
                                                server.strength = ARCHER_STRENGTH;
                                                server.speed = ARCHER_SPEED;
                                                break;
                                            }
                                        }
                                    }
                                }
                                
                                printf(" you need to decide what you will do now :\n");
		                        printf("p :physical attack\nm :magical attack\nf : give up\n");
		                        struct game_packet *d;
		                        d->magic_number = 0x55aa;
		                        d->pkt_len = 14;
		                        d->service = SERVICE_GAMEOP;
		                        struct game_op_packet *gameop2 = (struct game_op_packet *)d->data;
		                        char f = getchar();
		                        switch (f) {
		                            case 'p':gameop2->game_op = 0x01;break;
		                            case 'm':gameop2->game_op = 0x02;break;
		                            case 'f':Gameon = 0;gameop2->game_op = 0xc0;break;
		                        }
		                        gameop2->from = local;
		                        gameop2->to = server;
		                        send(sockfd, d, 22, 0);
		                        while(recv(sockfd, recvline2 , MAXLINE, 0) != 0) {
		                            struct game_packet *packet = (struct game_packet *)recvline2;
		                            if((packet->magic_number == 0x55aa) && (packet->service == SERVICE_GAMEOP)){
		                                struct game_op_packet *gameop = (struct game_op_packet *)packet->data;
		                                //printf
		                                struct_gameop_print(gameop);
		                                //now your turn 
		                                printf(" you need to decide what you will do now :\n");
		                                printf("p :physical attack\nm :magical attack\nf : give up\n");
		                                struct game_packet *a;
		                                a->magic_number = 0x55aa;
		                                a->pkt_len = 14;
		                                a->service = SERVICE_GAMEOP;
		                                struct game_op_packet *gameop2 = (struct game_op_packet *)a->data;
		                                char c = getchar();
		                                switch (c) {
		                                    case 'p':gameop2->game_op = 0x01;
		                                    case 'm':gameop2->game_op = 0x02;
		                                    case 'f':gameop2->game_op = 0xc0;
		                                }
		                                gameop2->from = gameop->to;
		                                gameop2->to = gameop->from;
		                                send(sockfd, a, 22, 0);
		                                if(Gameon == 0) {
		                                    break;
		                                }
		                            }    
		                        }           
		                        if(Gameon == 0) {
		                            break;
		                        }
                            } 
                        }
                    }
                    if(Gameon == 1){
                    }
                    Gameon = 0;
                }
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
        memset(sendline, 0, sizeof(sendline));
        memset(sendline1, 0, sizeof(sendline1));
        memset(sendline2, 0, sizeof(sendline2));
        memset(recvline1, 0, sizeof(recvline1));
        memset(recvline2, 0, sizeof(recvline2));
        memset(recvline3, 0, sizeof(recvline3));
        memset(recvdata, 0, sizeof(recvdata));
        memset(senddata, 0, sizeof(senddata));
        
        
        
	printf("Please log in(started with '#' or the name will not be accepted\n");	

	while (fgets(sendline, MAXLINE, stdin) != NULL) {
	        //log in
            if (sendline[0] == '#') {
                struct game_packet packet;
                packet.magic_number = 0x55aa;
                packet.service = SERVICE_LOGIN;
                packet.pkt_len = strlen(sendline)-2;
                for(setfor_for = 0; setfor_for < packet.pkt_len; setfor_for++) {
                    packet.data[setfor_for] = sendline[setfor_for + 1];
                    selfname[setfor_for] = sendline[setfor_for + 1];
                }
                send(sockfd, &packet, packet.pkt_len + 8, 0);
                printf("you have logged in and your name is %s\n",selfname);
                
                printf("you have several choice : 1.R means request to sb then you should type in a name\n");
                printf("                          2.C+words+#+name means chats to sb with these words\n");
                printf("                          3.c+words means chats to everyone with these words\n");
                printf("                          4.L means that you want to logout and maybe re-login\n");     
                
                pthread_t threads[4];
		pthread_create(&threads[0],NULL,get1,(void *)sockfd);
		pthread_create(&threads[1],NULL,get2,(void *)sockfd);
		pthread_create(&threads[2],NULL,get3,(void *)sockfd);
		pthread_create(&threads[3],NULL,get4,(void *)sockfd); 
            }
        }
	close(sockfd);
	return 0;
}


