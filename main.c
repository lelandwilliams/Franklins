#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
//#include "franklinlib.c"
#include "state.h"

void usage() {printf("Usage: franklin -n <nodes to create> [-p <port of neighbor>]\n");}

int main(int argc, char** argv) {
    srand(time(0));
    char ch;
    int n = -1;
    state_t state = new_state;
    state.my_id = rand() % 100000 +1;

    if(argc == 1) {
        usage();
        return 0;
    }
    while ((ch = getopt(argc, argv, "n:p:")) != EOF)
        switch(ch) {
            case 'n':
                n = strtol(optarg, NULL, 10);
                break;
            case 'p':
                state.L_port = strtol(optarg, NULL, 10);
                break;
            default:
                fprintf(stderr, "Unknown option: '%s'\n", optarg);
                usage();
        }
    argc -= optind;
    argv += optind;

    if(n == -1) {
        usage();
        return 0;
    }

    return 0;
}
