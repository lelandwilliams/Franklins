typedef enum {ACTIVE, PASSIVE, LEADER} nodestatus;

typedef struct {
    int server_port;
    int L_port;
    int R_port;
    nodestatus status ;
    int round_number;
    int L_ID;
    int R_ID;
    int my_id;
} state_t;

