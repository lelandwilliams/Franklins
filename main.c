#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "state.h"
#include "network.c"

void usage() {printf("Usage: %s %s %s %s %s\n",
        "franklin",
        "-n <nodes to create>",
        "[-m <my node number>]",
        "[-o <port of first node>]",
        "[-p <port of neighbor>]"
        );}

void print_status() {
    int star_length = 30;
    printf("\n");
    for(int i = 0; i<star_length; i++)
        printf("*");
    printf("\n");

    printf("Node Number: %d of %d\n", id, num_nodes);
    printf("Franklin ID: %d\n", franklin_id);
    printf("Node 1 Port: %d\n", first_port);
    printf("This Node's Port: %d\n", server_port);
    printf("This Node's Address: %s\n", server_octets);
    printf("Previous Node's Port: %d\n", port_L);

    for(int i = 0; i<star_length; i++)
        printf("*");
    printf("\n\n");
}

int process_args(int argc, char** argv) {
    /*
     * Returns number of nodes to create, or -1 if 
     * missing -n switch
     * Also alters the value of state_t.L_port if
     * -p switch is given
     */
    char ch;

    if(argc == 1) 
        return 1;

    while ((ch = getopt(argc, argv, "m:n:o:p:")) != EOF)
        switch(ch) {
            case 'm':
                id = strtol(optarg, NULL, 10);
                break;
            case 'n':
                num_nodes = strtol(optarg, NULL, 10);
                break;
            case 'o':
                first_port = strtol(optarg, NULL, 10);
                break;
            case 'p':
                port_L = strtol(optarg, NULL, 10);
                //printf("Node %d reading %s as %d\n", id, optarg, port_L);
                break;
            default:
                fprintf(stderr, "Unknown option: '%s'\n", optarg);
                return 1;
        }
    argc -= optind;
    argv += optind;
    return 0;
}

int main(int argc, char** argv) {
    state_t state = new_state;

    // since this program will probably run on the same
    // machine, the random number generators will likely
    // interfere which each other because the startup time 
    // is so fast
    // that using time(0) can return the same result 
    // so using time as a seed causes the rng to 
    // produce the same value, so instead we draw
    // from /dev/urandom for a seed

    //read from /dev/urandom
    unsigned char rbuffer[8];
    int fd = open("/dev/urandom", O_RDWR);
    read(fd, rbuffer, 8);
    close(fd);

    unsigned long int seed = 0;
    for(int i = 0; i < 8; i++)
        seed = seed * 256 + rbuffer[i];
    srand48(seed);
    franklin_id = lrand48() % 1000000 +1;

    if(process_args(argc, argv)) {
        usage();
        return 0;
    }

    if (num_nodes == -1) {
        usage();
        return 0;
    }

    start_server(55336, &state);
    if(id == 1)
        first_port = server_port;
    print_status();

    if(num_nodes == id) 
        port_R = first_port;
    else {
        pid_t fork_result = fork();
        if(fork_result == -1)
            perror("Failed to fork") ;
        if(fork_result == 0) {
            char next_id[8];
            sprintf(next_id, "%d", (id +1));
            char num_nodes_s[8];
            sprintf(num_nodes_s, "%d", num_nodes);
            char port_s[8];
            sprintf(port_s, "%d", server_port);
            char original_s[8];
            sprintf(original_s, "%d", first_port);

            pid_t e_result = execl("./franklin", 
                    "franklin", 
                    "-m", next_id,
                    "-n", num_nodes_s, 
                    "-o", original_s,
                    "-p", port_s,
                    NULL);
            if(e_result == -1)
                perror("Exec Error");
        }
    }

    if(id > 1)
        establish(port_L);
    if(id == num_nodes)
        establish(port_R);

    main_loop(&state);
    return 0;
}

