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

int server_port;
int server_socket;
char* server_octets;
int parity;
int f_ids[4] = {0,0,0,0};
int election_round = 0;

struct sockaddr_in addr;

void election();
void establish(int);
int process_message(char*);
void send_message(message_t, int,int,int);
int start_server(int, state_t*);
void main_loop(state_t*);
#endif
