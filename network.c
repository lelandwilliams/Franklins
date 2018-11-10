#include "network.h"

void establish(int port) {
    send_message(PORT, port);
}

void main_loop(state_t *state) {
    int done = 0;
    //while(done == 0) 
     //   if(! network_complete()) {;}
}

void send_message(message_t type, int reciever) {
    char msg[10];
    char val[10];
    switch(type) {
        case PORT:
            strcat(msg, "P;");
            sprintf(val, "%d", server_port);
            break;
        case ELECTION:
            strcat(msg, "E;");
            sprintf(val, "%d", franklin_id);
            break;
        case DIE:
            strcat(msg, "D;");
            sprintf(val, "%d", 100);
            break;
        default:
            printf("send_message(): what am I doing here ?\n");
    }
    strcat(msg, val);
}

int start_server(int last_port, state_t *state) {
    /* 
     * */
    server_port = last_port;
    int done = 0; 

    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
        perror("Error creating a socket");
    //struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    while(!done) {
        server_port++;
        addr.sin_port = (in_port_t) htons(server_port);
        int result = bind(server_socket, (struct sockaddr *) &addr, sizeof(addr));
        if (result != -1)
            done = 1;
//        else
 //           fprintf(stderr, "Can't bind socket to port\n", strerror(errno));
    }

    server_octets = inet_ntoa(addr.sin_addr);
    return 0;
}


