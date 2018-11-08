#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "state.h"
#include "network.c"

void usage() {printf("Usage: franklin -n <nodes to create> [-p <port of neighbor>]\n");}

int process_args(int argc, char** argv, state_t *st) {
    /*
     * Returns number of nodes to create, or -1 if 
     * missing -n switch
     * Also alters the value of state_t.L_port if
     * -p switch is given
     */
    char ch;
    int n = -1;

    if(argc == 1) 
        return n;

    while ((ch = getopt(argc, argv, "n:p:")) != EOF)
        switch(ch) {
            case 'n':
                n = strtol(optarg, NULL, 10);
                break;
            case 'p':
                st->L_port = strtol(optarg, NULL, 10);
                break;
            default:
                fprintf(stderr, "Unknown option: '%s'\n", optarg);
                usage();
        }
    argc -= optind;
    argv += optind;

    return n;
}

int main(int argc, char** argv) {
    srand(time(0));
    state_t state = new_state;
    state.my_id = rand() % 100000 +1;

    int num_nodes = process_args(argc, argv, &state);
    if (num_nodes == -1) {
        usage();
        return 0;
    }

    start_server(55336, &state);
    return 0;
}

