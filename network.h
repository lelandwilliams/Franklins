#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "state.h"

typedef enum{PORT, ELECTION, DIE} message_type;

int server_port;
int server_socket;
char* server_octets;

struct sockaddr_in addr;

void establish(int);
int start_server(int, state_t*);
void main_loop(state_t*);
#endif
