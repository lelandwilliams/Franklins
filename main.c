#include <stdio.h>
#include <unistd.h>
#include "franklinlib.c"
#include "state.c"

int main(int argc, char** argv) {
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
                n = optarg;
                break;
            case 'p':
                state.L_port = optarg;
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
