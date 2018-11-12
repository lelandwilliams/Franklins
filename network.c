#include "network.h"

void election(){;}

void establish(int port) {
    send_message(PORT, port);
}

void main_loop(state_t *state) {
    printf("Node %d in  main_loop()\n", id);
    int done = 0;
    int bytes_read = 0;
    char buffer[50];

    if(listen(server_socket, 3) == -1){
        perror("Cannot listen on Socket");
        done = 1;
    }
    printf("Node %d listening\n", id);

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
            printf("Node %d received a message\n", id);
            bytes_read = recv(client_socket, buffer, 49, 0);
            if(bytes_read < 0) {
                sprintf(buffer, "Node %d: Err on recv()\n", id);
                perror(buffer);
            }// if
            else
                done = process_message(buffer);
        } //else
    } // while
    close(server_port);
}// main_loop()

int process_message(char* msg) {
    printf("Node %d revieved message '%s'\n", id, msg);
    int incoming_id = -1;

    char *message_type = strtok(msg, ";");
    char *port_s = strtok(msg, ";");
    int incoming_port = strtol(port_s, NULL, 10);
    char *id_s = strtok(msg, ";");
    if(id_s != NULL)
        incoming_id = strtol(id_s, NULL, 10);

    switch(message_type[0]) {
        case 'P':
            if(!port_L && !port_R)
                port_R = incoming_port;
            if(!port_R)
                port_L = incoming_port;
            if(port_L && port_R) {
                send_message(ELECTION, port_L);
                send_message(ELECTION, port_R);
            }
            break;
        case 'D':
            break;
    }
}

void send_message(message_t type, int receiver) {
    int out_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in out_addr;
    memset(&out_addr, 0, sizeof(out_addr));
    out_addr.sin_family = PF_INET;
    out_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    out_addr.sin_port = htons(receiver);
    int result = connect(out_socket, 
            (struct sockaddr *) &out_addr, 
            sizeof(out_addr));
    if(result == -1) {
        char e[50];
        sprintf(e, "Node %d: Connection Error to port %d", id, receiver);
        perror(e);
    }

    char msg[50];
    msg[0] = '\0';
    char val[30];
    switch(type) {
        case PORT:
            strcat(msg, "P;");
            sprintf(val, "%d", server_port);
            break;
        case ELECTION:
            strcat(msg, "E;");
            sprintf(val, "%d;%d;%d", server_port,franklin_id, election_round %2 );
            break;
        case DIE:
            strcat(msg, "D;");
            sprintf(val, "%d", server_port);
            break;
        default:
            printf("Node %d: Reached end of send_message() switch", id);
    }
    strcat(msg, val);
    int bytes_sent = 0;
    while(bytes_sent == 0) {
        printf("Node %d sending '%s' to port %d\n", id, msg, receiver);
        bytes_sent = send(out_socket, msg, strlen(msg), 0);
    }
    printf("Node %d sent %d bytes to port %d\n", id, bytes_sent, receiver);
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


