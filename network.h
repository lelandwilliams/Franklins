#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "state.h"

typedef enum{PORT, ELECTION, DIE} message_t;

int leader_count = 0;
int server_port = 0;
int server_socket = 0;
char* server_octets = NULL;
int f_ids[4] = {0,0,0,0}; // keeps track of which franklin ranks
                          // the node has recieved for each parity
int election_round = 0;
int f_sent[4] = {0,0,0,0}; // keeps track of which nodes this node
                            // has sent info two in each parity

struct sockaddr_in addr;

void establish(int);
int process_message(char*);
void send_message(message_t, int,int,int);
void send_election(int, int);
int start_server(int);
void main_loop();
#endif
