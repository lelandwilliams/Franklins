#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "state.h"

typedef enum{PORT, ELECTION, DIE} message_t;
//typedef enum {SENDING, WAITING, DONE} election_phase;
typedef enum {LEFT, RIGHT} direction;

typedef struct msg_t{ 
    direction d; 
    int id;
    int parity;
//    int origen;
} msg;


msg *message_q[10] = {NULL};
const int q_size = 10;

int done = 0; // state value that causes main_loop to exit
int leader_count = 0;
int server_port = 0;
int server_socket = 0;
char* server_octets = NULL;
int f_ids[2][2] = {{0}}; // keeps track of which franklin ranks
                          // the node has recieved for each parity
                          // from the left and the right
int election_round = 0; // parity of current election
//int f_sent[2] = {0}; // keeps track of which nodes this node
                     // has sent its election info for each round

struct sockaddr_in addr;

direction send_direction = LEFT; // send routing will switch between trying
                                 // to send messages to the left and right
                                 // in case one way is blocked.

void enque_message(direction, int, int);
void establish(int);
void mind_business();
void process_message(char*);
int send_message(message_t, int,int,int);
void send_election(int, int);
int start_server(int);
void main_loop();
#endif
