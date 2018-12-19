#ifndef STATE_H
#define STATE_H

typedef enum {LISTENING, ACTIVE, PASSIVE, LEADER} nodestatus;
int franklin_id;
int id = 1;
int first_port = -1; // remembers port of first node
int port_L = 0; // the port # of the node to the 'left', or id -1
int port_R = 0; // the port # of the node to the 'right', or id +1
int num_nodes = 0;
nodestatus status;

void print_status();
struct state_s{
    int L_port;
    int R_port;
    nodestatus status;
    int round_number;
    int my_id;
} new_state = {0,0,ACTIVE,0,0};

typedef struct state_s state_t;

#endif
