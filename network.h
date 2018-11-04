#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include "state.h"

int serv_sock;
int start_server(int, state_t*);
#endif
