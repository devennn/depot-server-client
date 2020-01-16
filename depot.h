#ifndef DEPOT_H
#define DEPOT_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

// Constant value
#define DELIVER_FORMAT 3
#define DEFER_FORMAT 6
#define EXECUTE_FORMAT 2
#define LENGTH 1024
#define IM_FORMAT 3
#define TRANSFER_FORMAT 4

// For exit program usage
typedef enum {
    END_NORMAL = 0,
    INVALID_ARG = 1,
    INVALID_NAME = 2,
    INVALID_QUANTITY = 3,
} ExitMessage;

// For message identification.
// WRONG_FORMAT is to identify bad format input and invalid message format
typedef enum {
    WRONG_FORMAT = 0,
    CONNECT = 1,
    IM = 2,
    DELIVER = 3,
    WITHDRAW = 4,
    TRANSFER = 5,
    DEFER = 6,
    EXECUTE = 7,
} MessageType;

// Every single depot variables
typedef struct {
    // thread id
    pthread_t tid;
    // file pointer for reading input
    FILE *receive;
} Depot;

typedef struct {
    // Store depot name
    char *depotName;
    // Store all goods processed
    char **goods;
    // store quantity for goods
    int *qty;
    // number of items currently have
    int numOfItems;
    // Input message
    char *mes;
    // length of string of goods name
    int goodsStrLen;
    // Connection file descriptor
    int connFd;
    // store defer message key
    int *key;
    // Store defer message
    char **deferMes;
    // store neighbour name
    char **nbr;
    // store number of neighbour
    int numOfNbr;
    // stre number of defer messages
    int numOfDefer;
    // struct Depot
    Depot depot;
} Info;

// GLobal struct for signal handler usage and tracking
typedef struct {
    // Store all goods processed
    char **goods;
    // store quantity for goods
    int *qty;
    // store neighbour name
    char **nbr;
    // number of items currently have
    int numOfItems;
    // thread counter
    int thrCtr;
    // store number of neighbour
    int numOfNbr;
    // store file descriptor for every neighbour
    int *fd;
    // store port number
    uint16_t portNum;
    // Struct Depot
    Depot **depot;
    // file pointer for who is currently active
    FILE *now;
    // store connection message history
    char **conHist;
    // store number of connection
    int numOfCon;
} Handle;
extern Handle h;

/*
* General function for depot
*/
void exit_program(int status);
void parse_args(int argc, const char *argv[], Info *info);
void goods_quantity_check(int argc, const char *argv[], Info *info);
bool valid_number(const char *goodsQty);
bool valid_names(char *name);
void allocate_space(int numOfItem, Info *info);
void free_allocated_space(int numOfItem, Info *info);

/*
* For setting up network
*/
int setup_network(Info *info);
bool port_info(int sockfd, Info *info);
void accept_connection(int sockfd, Info *info);

/*
* Threading
*/
bool stream_reading(Info *info);
void start_threading(Info *info);
void global_realloc(int n);
void *thread_entry(void *arg);

/*
* For message error checking
*/
void process_message(Info *info);
void update_signal_handler(Info *info);
MessageType message_format(Info *info);
bool correct_message_arg(int numOfSemColon, int mesType);
MessageType which_message(char *buf);
bool correct_message_format(char *mes, Info *info);
bool end_check(char buf);
bool correct_parameters(char split[][LENGTH], Info *info);
bool deliver_withdraw_check(char split[][LENGTH]);
bool check_connection_hist(char *newCon);
bool im_check(char split[][LENGTH]);
bool transfer_check(char split[][LENGTH]);
bool defer_check(char split[][LENGTH], Info *info);

/*
* For processing message
*/

// CONNECT
void connect_depot(Info *info);
int connect_process(char **par);

// Introduction, IM
void send_im_mes(Info *info, int connFd);
void receive_im_message(Info *info);
bool check_neighbour_exists(Info *info, char *par, int num);

// DELIVER and WITHDRAW
void deliver(Info *info);
void deliver_process(Info *info, char **par, int qty);
void withdraw(Info *info);
void withdraw_process(Info *info, char **par, int qty);
void add_goods(Info *info);

// DEFER
void defer(Info *info);
void defer_process(Info *info, char **par);

// EXECUTE
void execute(Info *info);
void execute_process(Info *info, int pos);
void run_defer_mes(Info *info);

// TRANSFER
void transfer(Info *info);
int find_depot_port(Info *info, char *depotName);
void transfer_process(Info *info, char *goods, int qty, int fd);

// For formatting messages
void format_message(Info *info);
void remove_carriage_return_end(Info *info);
int break_message(Info *info, char **par);
void sort_lexiographic_order(Info *info, int status);
void free_par(char **par, int ttlAllocated);

#endif
