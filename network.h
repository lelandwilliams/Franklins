#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "state.h"

int server_port;
int server_socket;
char* server_octets;

struct sockaddr_in addr;

int start_server(int, state_t*);
#endif
