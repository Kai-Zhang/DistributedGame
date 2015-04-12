#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <protocol.h>
#include <character.h> 
#include <unistd.h>

#define MAXLINE 128

static int status; 
char selfname[32];
char destname[32];
int setfor_for = 0;
char sendline[MAXLINE],recvline[MAXLINE],recvdata[MAXLINE],senddata[MAXLINE];
struct player self,dest;


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
//print player
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

//print gameop and two players
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
		    break;
		    };
		case 0xc0: {
		    printf("you have lost !\n");
		    break;
		    };
	}
}

 
 void *get0(void *conn){
     int sockfd=*(int*)conn;
     while (fgets(sendline, MAXLINE, stdin) != NULL) {
         //send log in
         if((status == 0)&&(sendline[0] == '#')){
             if (sendline[0] == '#') {
                  status = 1;
                  struct game_packet packet;
                  packet.magic_number = 0x55aa;
                  packet.service = 0x01;
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
                  printf("                          4.L means that you want to log out and maybe re-log in\n"); 
                    
                  
             }
         }
         //send broadcast
         else if((status != 0)&&(sendline[0] == 'c')) {
             struct game_packet packet;
             packet.magic_number = 0x55aa;
             packet.service = 0x08;
             packet.pkt_len = strlen(sendline) - 1;
             for(setfor_for = 0; setfor_for < packet.pkt_len; setfor_for++) {
                 packet.data[setfor_for] = sendline[setfor_for + 1];
             }
             send(sockfd, &packet, packet.pkt_len + 8, 0);
         }
         //send chat with one
         else if((status !=0)&&(sendline[0] == 'C')) {
             struct game_packet packet;
             packet.magic_number = 0x55aa;
             packet.service = 0x08;
             packet.pkt_len = strlen(sendline) - 2;
             for(setfor_for = 0; setfor_for < packet.pkt_len; setfor_for++) {
                 if(sendline[setfor_for + 1] == '#') {
                     packet.data[setfor_for] = '\0';
                 }
                 else {
                     packet.data[setfor_for] = sendline[setfor_for + 1];
                 }
             }
             send(sockfd, &packet, packet.pkt_len + 8, 0);
         }
 
         //send log out
         else if((sendline[0] == 'L')&&(status != 0)) {
             status = 0;
             struct game_packet packet;
             packet.magic_number = 0x55aa;
             packet.service = 0x02;
             packet.pkt_len = 0;
             send(sockfd, &packet, 8, 0);
             printf("you have logged out\n");
             printf("Please log in(started with '#' or the name will not be accepted)\n");
         }
         //send game request
         else if((sendline[0] == 'R') && (status == 1)) {
             status = 2;
             struct game_start_packet packet;
             packet.magic_number = 0x55aa;
             packet.service = 0x10;
             packet.pkt_len = 64;
             fgets(packet.data1, 32, stdin);
			 packet.data1[strlen(packet.data1)-1] = 0;
             strcpy(packet.data2,selfname);
             send(sockfd, &packet, 72, 0);
             printf("wait the reply\n");
         }
         //to be warrior
         else if((sendline[0] == 'w') && ((status == 6) || (status == 5))) {
             printf("you choose warrior\n");
             struct game_packet a;
             a.magic_number = 0x55aa;
             a.service = 0x18;
             a.pkt_len = 26;
             struct game_create_packet *gcpacket = (struct game_create_packet *)a.data;
             gcpacket->game_op = GAME_CHR_CREATE;
             gcpacket->character = PRO_WARRIOR;
             gcpacket->health_point = WARRIOR_MAX_HP;
             gcpacket->magic_point = WARRIOR_MAX_MP;
             gcpacket->defense = WARRIOR_DEFENSE;
             gcpacket->strength = WARRIOR_STRENGTH;
             gcpacket->speed = WARRIOR_SPEED;
             self.character = PRO_WARRIOR;
             self.health_point = WARRIOR_MAX_HP;
             self.magic_point = WARRIOR_MAX_MP;
             self.defense = WARRIOR_DEFENSE;
             self.strength = WARRIOR_STRENGTH;
             self.speed = WARRIOR_SPEED;
             send(sockfd, &a, 34, 0);
             status += 2;
         }
         //to be magician
         else if((sendline[0] == 'm') && ((status == 6) || (status == 5))) {
             printf("you choose magician\n");
             struct game_packet a;
             a.magic_number = 0x55aa;
             a.service = SERVICE_GAMEOP;
             a.pkt_len = 26;
             struct game_create_packet *gcpacket = (struct game_create_packet *)a.data;
             gcpacket->game_op = GAME_CHR_CREATE;
             gcpacket->character = PRO_MAGICIAN;
             gcpacket->health_point = MAGICIAN_MAX_HP;
             gcpacket->magic_point = MAGICIAN_MAX_MP;
             gcpacket->defense = MAGICIAN_DEFENSE;
             gcpacket->strength = MAGICIAN_STRENGTH;
             gcpacket->speed = MAGICIAN_SPEED;
             self.character = PRO_MAGICIAN;
             self.health_point = MAGICIAN_MAX_HP;
             self.magic_point = MAGICIAN_MAX_MP;
             self.defense = MAGICIAN_DEFENSE;
             self.strength = MAGICIAN_STRENGTH;
             self.speed = MAGICIAN_SPEED;
             send(sockfd, &a, 34, 0);
             status += 2;
         }
         //to be archer
         else if((sendline[0] == 'a') && ((status == 6) || (status == 5)))  {
             printf("you choose archer\n");
             struct game_packet a;
             a.magic_number = 0x55aa;
             a.service = SERVICE_GAMEOP;
             a.pkt_len = 26;
             struct game_create_packet *gcpacket = (struct game_create_packet *)a.data;
             gcpacket->game_op = GAME_CHR_CREATE;
             gcpacket->character = PRO_ARCHER;
             gcpacket->health_point = ARCHER_MAX_HP;
             gcpacket->magic_point = ARCHER_MAX_MP;
             gcpacket->defense = ARCHER_DEFENSE;
             gcpacket->strength = ARCHER_STRENGTH;
             gcpacket->speed = ARCHER_SPEED;
             self.character = PRO_ARCHER;
             self.health_point = ARCHER_MAX_HP;
             self.magic_point = ARCHER_MAX_MP;
             self.defense = ARCHER_DEFENSE;
             self.strength = ARCHER_STRENGTH;
             self.speed = ARCHER_SPEED;
             send(sockfd, &a, 34, 0);
             status += 2;
         }
         //give up or physical and magical attack
         else if(((sendline[0] == 'f') || (sendline[0] == 'p') || (sendline[0] == 'm')) && (status == 9)){
             struct game_packet a;
	         a.magic_number = 0x55aa;
	         a.pkt_len = 50;
	         a.service = SERVICE_GAMEOP;
	         struct game_op_packet *gameop2 = (struct game_op_packet *)a.data;
		     switch (sendline[0]) {
		         case 'p':gameop2->game_op = 0x01;status = 10;printf("you have done a physical attack\n"); break;
		         case 'm':gameop2->game_op = 0x02;status = 10;printf("you have done a magical attack\n"); break;
		         case 'f':gameop2->game_op = 0xc0;status = 1;printf("you gave up\n"); break;
		         default : ;
		     }
		     gameop2->from.character    = self.character;
		     gameop2->from.health_point = self.health_point;
		     gameop2->from.magic_point  = self.magic_point;
		     gameop2->from.defense      = self.defense;
		     gameop2->from.strength     = self.strength;
		     gameop2->from.speed        = self.speed;
		     gameop2->to.character      = dest.character;
		     gameop2->to.health_point   = dest.health_point;
		     gameop2->to.magic_point    = dest.magic_point;
		     gameop2->to.defense        = dest.defense;
		     gameop2->to.strength       = dest.strength;
		     gameop2->to.speed          = dest.speed;
		     send(sockfd, &a, 58, 0);
         }
         //send game on
         else if((sendline[0] == 'y') && (status == 3)) {
             status = 4;
             struct game_start_packet b;
             b.magic_number = 0x55aa;
             b.service = 0x11;
             b.pkt_len = 64;
             strcpy(b.data1, destname);
             strcpy(b.data2, selfname);
             send(sockfd, &b, 72, 0);
         }
         //send refuse
         else if((sendline[0] == 'n') && (status == 3)) {
             status = 1;
             struct game_start_packet b;
             b.magic_number = 0x55aa;
             b.service = 0x12;
             b.pkt_len = 64;
             strcpy(b.data1, destname);
             strcpy(b.data2, selfname);
             send(sockfd, &b, 72, 0);
         }
     }
 }
 
 void *get1(void *conn){
     int sockfd=*(int*)conn;
     while (recv(sockfd, recvline, MAXLINE, 0) != 0) {
         //usleep(100);	
         struct game_packet *packet = (struct game_packet *)recvline;
         if(packet->magic_number == 0x55aa) {
             //receive name list
             if((packet->service == 0x04) && (status != 0)) {
                 char *a = packet->data;
                 int i;
                 for(i = 0; i < packet->pkt_len; i += 32)
                 printf("%s\n", (a+i));
             }
             //receive chat
             else if(packet->service == 0x08) {
                 printf("chat :%s", packet->data);
             }
             //receive game request
             else if((packet->service == 0x10) && (status == 1)) {
                 status = 3;
				 struct game_start_packet *b = (struct game_start_packet *)recvline;
                 printf("if you want to play game with %s please type in a 'y' or you should type in an 'n'\n",b->data2);
				 strcpy(destname, b->data2);
				 strcpy(selfname, b->data1);
             }
             //receive game on and tell users to choose character
             else if( (packet->service == 0x11) && (status == 2)) {
                 printf("you can choose warrior,magician or archer by'w','m' or 'a'.\n");
				 struct game_start_packet b;
				 struct game_start_packet *m = (struct game_start_packet *)recvline;
				 puts(m->data1);
				 puts(m->data2);
            	 b.magic_number = 0x55aa;
            	 b.service = 0x11;
            	 b.pkt_len = 64;
            	 strcpy(b.data1, m->data2);
            	 strcpy(b.data2, m->data1);
            	 send(sockfd, &b, 72, 0);
                 status = 5;
             }
             //receive sad story
             else if((packet->service == 0x12) && (status == 2)) {
                 printf("you have been refused");
                 status = 1;
             }
             //receive game create
             else if((packet->service == 0x18) && ((status == 4)||(status == 7)) && (packet->pkt_len = 26)) {
                 struct game_create_packet *e = (struct game_create_packet *)packet->data;
                 if(e->game_op == GAME_CHR_CREATE) {
                     if(e->character == 0x1) {
                         if(status == 7) {
                             status = 9;
                             printf(" you need to decide what you will do now :\n");
		                     printf("p :physical attack\nm :magical attack\nf : give up\n");
                         }
                         if(status == 4) {
                             status = 6;
                             printf("you can choose warrior,magician or archer by 'w','m' or 'a'.\n");
                         }
                         dest.character = PRO_WARRIOR;
                         dest.health_point = WARRIOR_MAX_HP;
                         dest.magic_point = WARRIOR_MAX_MP;
                         dest.defense = WARRIOR_DEFENSE;
                         dest.strength = WARRIOR_STRENGTH;
                         dest.speed = WARRIOR_SPEED;
                     }
                     if(e->character == 0x2) {
                         if(status == 7) {
                             status = 9;
                             printf(" you need to decide what you will do now :\n");
		                     printf("p :physical attack\nm :magical attack\nf : give up\n");
                         }
                         if(status == 4) {
                             status = 6;
                             printf("you can choose warrior,magician or archer by 'w','m' or 'a'.\n");
                         }
                         dest.character = PRO_MAGICIAN;
                         dest.health_point = MAGICIAN_MAX_HP;
                         dest.magic_point = MAGICIAN_MAX_MP;
                         dest.defense = MAGICIAN_DEFENSE;
                         dest.strength = MAGICIAN_STRENGTH;
                         dest.speed = MAGICIAN_SPEED;
                     }
                     if(e->character == 0x3) {
                         if(status == 7) {
                             status = 9;
                             printf(" you need to decide what you will do now :\n");
		                     printf("p :physical attack\nm :magical attack\nf : give up\n");
                         }
                         if(status == 4) {
                             status = 6;
                             printf("you can choose warrior,magician or archer by 'w','m' or 'a'.\n");
                         }
                         dest.character = PRO_ARCHER;
                         dest.health_point = ARCHER_MAX_HP;
                         dest.magic_point = ARCHER_MAX_MP;
                         dest.defense = ARCHER_DEFENSE;
                         dest.strength = ARCHER_STRENGTH;
                         dest.speed = ARCHER_SPEED;
                     }
                 }
             }
             //receive game op
             else if((packet->service == 0x18) && ((status == 8)||(status == 10)) && (packet->pkt_len = 50)){
                 status = 9;
                 struct game_op_packet *gameop = (struct game_op_packet *)packet->data;
		         struct_gameop_print(gameop);
                 printf(" you need to decide what you will do now :\n");
		         printf("p :physical attack\nm :magical attack\nf : give up\n");
             }
             
             else if(packet->service == 0x80){
                 status = 1;
                 printf("you have won\nnow you can launch a new request\n");
             }
             else if(packet->service == 0xc0){
                 status = 1;
                 printf("you have lost\nnow you can launch a new request\n");
             }
         }
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

        memset(sendline, 0, sizeof(sendline));  
        memset(recvline, 0, sizeof(recvline));
        memset(recvdata, 0, sizeof(recvdata));
        memset(senddata, 0, sizeof(senddata));
        
        
      
                
        pthread_t threads[2];
		pthread_create(&threads[0],NULL,get0,(void *)&sockfd);
		pthread_create(&threads[1],NULL,get1,(void *)&sockfd);
		printf("Please log in(started with '#' or the name will not be accepted)\n");
		while(1);
       
	close(sockfd);
	return 0;
}


