#include "network.h"

int start_server(int last_port, state_t *state) {
    /* 
     * */
    state->server_port = last_port;
    int done = 0; 

    state->server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (state->server_socket == -1)
        perror("Error creating a socket");
    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    while(!done) {
        state->server_port++;
        addr.sin_port = (in_port_t) htons(state->server_port);
        int result = bind(state->server_socket, (struct sockaddr *) &addr, sizeof(addr));
        if (result == 0)
            done = 1;
    }

    return 0;
}


