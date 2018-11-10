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

void print_status(state_t *st) {
    int star_length = 30;
    printf("\n");
    for(int i = 0; i<star_length; i++)
        printf("*");
    printf("\n");

    printf("Node Number: %d\n", id);
    printf("Franklin ID: %d\n", franklin_id);
    printf("Original Port: %d\n", first_port);
    printf("Server Port: %d\n", server_port);
    printf("Server Address: %s\n", server_octets);

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
                break;
            default:
                fprintf(stderr, "Unknown option: '%s'\n", optarg);
                usage();
                return 1;
        }
    argc -= optind;
    argv += optind;
    return 0;
}

int main(int argc, char** argv) {
    srand(time(0));
    state_t state = new_state;
    franklin_id = rand() % 100000 +1;

    if(process_args(argc, argv))
        return 0;

    if (num_nodes == -1) {
        usage();
        return 0;
    }

    start_server(55336, &state);
    if(id == 1)
        first_port = server_port;
    print_status(&state);

    if(num_nodes == id) 
        port_R = first_port;
    else {
        pid_t fork_result = fork();
        if(fork_result == -1)
            perror("Failed to fork") ;
        if(fork_result == 0) {
            char next_id[5];
            sprintf(next_id, "%d", (id +1));
            char num_nodes_s[5];
            sprintf(num_nodes_s, "%d", (num_nodes));
            char port_s[5];
            sprintf(port_s, "%d", (server_port));
            char original_s[5];
            sprintf(original_s, "%d", (first_port));

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
    close(server_socket);
    return 0;
}

