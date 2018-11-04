#ifndef STATE_H
#define STATE_H

typedef enum {ACTIVE, PASSIVE, LEADER} nodestatus;

struct state_s{
    int server_port;
    int L_port;
    int R_port;
    nodestatus status ;
    int round_number;
    int my_id;
} new_state = {0,0,0,ACTIVE,0,0};

typedef struct state_s state_t;

#endif
