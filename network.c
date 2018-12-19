#include "network.h"

void enque_message(direction d, int f_id, int parity) {
    int message_idx = -1; // -1 indicates no free space.
    for(int i=0;i<q_size;i++)
        if(!message_q[i] && message_idx == -1)
            message_idx = i;

    if(message_idx == -1) {
        printf("Node %d No Free Message Space\n", id);
        return;
    }

    msg *m = (msg *)malloc(sizeof(msg));
    if(!m) {
        printf("Node %d Failure to allocate memory\n", id);
        return;
    }

    m->d = d;
    m->id = f_id;
    m->parity = parity;
    message_q[message_idx] = m;
    //printf("Node %d place a message in slot %d\n", id, message_idx);
}

void establish(int port) {
    send_message(PORT, port, 0, 0);
}

void main_loop() {
    int bytes_read = 0;
    char buffer[50];
    struct sockaddr_storage client_addr;
    int client_addr_size = sizeof(client_addr);

    while(done == 0) {
        //printf("Node %d checking for incoming\n", id);
        int client_socket = accept(server_socket, 
                (struct sockaddr *) &client_addr,
                &client_addr_size);
        if(client_socket == -1){
            if(errno != EAGAIN || errno != EWOULDBLOCK) {
                sprintf(buffer, "Node %d: Err accepting connection\n", id);
                perror(buffer);
            }
        }
        else {
            //printf("Node %d received a message\n", id);
            bytes_read = recv(client_socket, buffer, 49, 0);
            if(bytes_read < 0) {
                sprintf(buffer, "Node %d: Err on recv()\n", id);
                perror(buffer);
            }// if
            else {
                process_message(buffer);
                buffer[0] = '\0';
            }
        } //else
        close(client_socket);
        mind_business();
        sleep(2);
        //usleep(800000);
    } // while (done == 0)
    close(server_port);
}// main_loop()

void mind_business() {
    /* 
     * This routine checks to see if there any messages 
     * that are due for sending, if so it sends one out.
     * Otherwise it processes any election rounds
     */
    
    // check if there are messages.
    int message_idx = -1; // -1 indicates no messages.
    for(int i=0;i<q_size;i++)
        if(message_q[i] != NULL && message_idx == -1)
            message_idx = i;

    //if(message_idx == -1)
        //printf("Node %d has no messages to send\n", id);

    if(message_idx != -1) {
        //printf("Node %d has a message in slot %d\n", id, message_idx);
        int reciever = port_L;
        if(message_q[message_idx]->d == RIGHT)
            reciever = port_R;
        int m_id = message_q[message_idx]->id; 
        int parity = message_q[message_idx]->parity; 
        int failure = send_message(ELECTION, reciever, m_id, parity);
        if(!failure) {
            free(message_q[message_idx]);
            message_q[message_idx] = NULL; 
        }
        return;
    }

    // have ids from both sides
    if(status != ACTIVE)
        return;

    if( (f_ids[election_round][0] > 0)  
            && (f_ids[election_round][1]) > 0) { 
        printf("Node %d has franklin value %d which it compares against %d and %d\n", 
                id, 
                franklin_id,  
                f_ids[election_round][0],
                f_ids[election_round][1]);
        if(franklin_id > f_ids[election_round][0] && 
                franklin_id > f_ids[election_round][1]) {
            // This node won the election
            printf("Node: %d compares franklin ids and remains in the election\n\n", id);
            f_ids[election_round][0] = 0;
            f_ids[election_round][1] = 0;
            election_round = (election_round + 1) % 2;
            enque_message(LEFT, franklin_id, election_round);
            enque_message(RIGHT, franklin_id, election_round);
        } //if (franklin_id ...)
        else
        {
            printf("Node: %d compares franklin ids and leaves the election\n", id);
            status = PASSIVE;
            // check to see if any next round messages have come
            // in and if so, pass them along.
            election_round = (election_round + 1) % 2;
            if(f_ids[election_round][0])
                enque_message(RIGHT, f_ids[election_round][0], election_round);
            if(f_ids[election_round][1])
                enque_message(LEFT, f_ids[election_round][1], election_round);
        }//else
    }//if(f_ids ...)
}// mind_business()

void process_message(char* msg) {
    int incoming_id = -1;
    int parity = -1;

    char *message_type = strtok(msg, ";");
    char *next_s = strtok(NULL, ";");
    int incoming_port = strtol(next_s, NULL, 10);
    //printf("Node %d recieved message '%s' from port %d\n", id, msg, incoming_port);
    next_s = strtok(NULL, ";");
    if(next_s != NULL)
        incoming_id = strtol(next_s, NULL, 10);
        next_s = strtok(NULL, ";");
        if(next_s != NULL)
            parity = strtol(next_s, NULL, 10);

    switch(message_type[0]) {
        case 'P':
            //printf("Node %d recieved a PORT message\n", id);
            if(!port_R)
                port_R = incoming_port;
            else
                port_L = incoming_port;
            //usleep(250000);

            if(port_L && port_R) {
                print_status();
                printf("Node %d: has connected to its ring and begins election\n", id);
                //send_election(port_L, 0);
                //usleep(250000);
                //send_election(port_R, 0);
            }
            break;

        case 'E':
            //printf("Node %d recieved an election message\n", id);
            if(incoming_id == franklin_id) {
                leader_count++;
                if(leader_count == 2) {
                    status = LEADER;
                    printf("Node: %d received it's own id so is now leader\n", id);
                    send_message(DIE, port_R, server_port, server_port);
                }// if
                break;
            }// if
            if(status == PASSIVE)  {
                printf("Node %d received an election message, but is passive so passing along\n", id);
                if(incoming_port == port_L)
                    enque_message(RIGHT, incoming_id, parity);
                else
                    enque_message(LEFT, incoming_id, parity);
            }
            if(status == ACTIVE) {
                // add message to election messages for later comparison;
                int slot = 0;
                if(incoming_port == port_R)
                    slot = 1;
                f_ids[parity][slot] = incoming_id;
                /*printf("Node %d has f_ids (%d, %d)\n", id, 
                        f_ids[parity][0],
                        f_ids[parity][1]);*/
            }// if (status == ACTIVE)
            break;
        case 'D':
            //printf("Node %d recieved a DIE message\n", id);
            if (incoming_id == port_R)
                printf("Node %d is the last so not passing along DIE message\n",id);
            else {
                printf("Node %d passing along Die message\n",id);
                send_message(DIE, port_R, incoming_id, -1);
            }
            done = 1;
            break;
    }
}

int send_message(message_t type, int dest, int payload, int parity) {
    // a return val of 1 indicates failure to send
    int out_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (out_socket == -1) {
        perror("Error creating an outgoing socket");
        return 1;
    }
    
    // set the waiting time 
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 250000;
    
    //set the socket to have a timeout
    if(setsockopt(out_socket,SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout))) {
        perror("Unable to set out_socket to timeout");
        return 1;
    }

    struct sockaddr_in out_addr;
    memset(&out_addr, 0, sizeof(out_addr));
    out_addr.sin_family = PF_INET;
    out_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    out_addr.sin_port = htons(dest);
    int result = connect(out_socket, 
            (struct sockaddr *) &out_addr, 
            sizeof(out_addr));
    if(result == -1) {
        char e[50];
        sprintf(e, "Node %d: Connection Error to port %d", id, dest);
        perror(e);
        return 1;
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
            sprintf(val, "%d;%d;%d", server_port, payload, parity);
            break;
        case DIE:
            strcat(msg, "D;");
            sprintf(val, "%d;%d", server_port, payload);
            break;
        default:
            printf("Node %d: Reached end of send_message() switch", id);
    }
    strcat(msg, val);
    int bytes_sent = 0;
    
    while(bytes_sent == 0) {
        bytes_sent = send(out_socket, msg, strlen(msg), 0);
    }
    //printf("Node %d sent %d bytes to port %d\n", id, bytes_sent, dest);
    //printf("Node %d sent '%s' to port %d\n", id, msg, dest);

    close(out_socket);
    return 0;
}

int start_server(int last_port) {
    /* 
     * */
    server_port = last_port;
    int done = 0; // done when server port is bound
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // create a socket and give it a timeout
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
        perror("Error creating a socket");
    // set the waiting time 
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 250000;
    //set the socket to have a timeout
    if(setsockopt(server_socket,SOL_SOCKET,SO_RCVTIMEO, (char *) &timeout, sizeof(timeout)))
        perror("Unable to set server_socket to timeout");

    // find an available port
    while(!done) {
        server_port++;
        addr.sin_port = (in_port_t) htons(server_port);
        int result = bind(server_socket, (struct sockaddr *) &addr, sizeof(addr));
        if (result != -1)
            done = 1;
    }

    server_octets = inet_ntoa(addr.sin_addr);

    if(listen(server_socket, 3) == -1){
        perror("Cannot listen on Socket");
        return 1;
    }
    printf("Node %d listening\n", id);

    return 0;
}


