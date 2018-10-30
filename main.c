#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
//#include "franklinlib.c"
#include "state.c"

int main(int argc, char** argv) {
    srand(time(0));
    char ch;
    int n = -1;
    state_t state;

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
