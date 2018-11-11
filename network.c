#include "network.h"

void establish(int port) {
    send_message(PORT, port);
}

void main_loop(state_t *state) {
    int done = 0;
    int bytes_read = 0;
    char buffer[50];
    if(listen(server_socket, 3) == -1)
        perror("Cannot listen on Socket");
    struct sockaddr_storage client_addr;
    int client_addr_size = sizeof(client_addr);

    while(done == 0) {
        int client_socket = accept(server_socket, 
                (struct sockaddr *) &client_addr,
                &client_addr_size);
        if(client_socket == -1){
            sprintf(buffer, "Node %d: Err accepting connection\n", id);
            perror(buffer);
        }
        else {
            bytes_read = recv(client_socket, buffer, 49, 0);
            if(bytes_read < 0) {
                sprintf(buffer, "Node %d: Err on recv()\n", id);
                perror(buffer);
            }// if
            else
                done = process_message(buffer);
        } //else
    } // while
}// main_loop()

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


