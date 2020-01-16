#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>

#include "depot.h"

// Mutex lock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/*
* Exit program when error happens. Take arguments of status int and trigger
* status error accordingly
*
* Return nothing
*/
void exit_program(int status)
{
    switch(status) {

        case INVALID_ARG:
            fprintf(stderr, "Usage: 2310depot name {goods qty}\n");
            exit(status);
            break;

        case INVALID_NAME:
            fprintf(stderr, "Invalid name(s)\n");
            exit(status);
            break;

        case INVALID_QUANTITY:
            fprintf(stderr, "Invalid quantity\n");
            exit(status);
            break;
    }
}

/*
* Parse argument. Take arguments of arguments count, program arguments and
* struct Info. This function is the initial check program input. If invalid
* input exist, program will trigger exit program
*
* Return Nothing
*/
void parse_args(int argc, const char *argv[], Info *info)
{
    //arguments must have goods and quantity
    if(((argc - 2) % 2 != 0) || argc < 2) {
        free_allocated_space(argc - 2, info);
        exit_program(INVALID_ARG);
    }
    info->numOfItems = (argc - 2) / 2;
    //check depot name. if valid, store it
    if(!valid_names((char *)argv[1])) {
        free_allocated_space(argc - 2, info);
        exit_program(INVALID_NAME);
    } else {
        strcpy(info->depotName, argv[1]);
    }
    goods_quantity_check(argc, argv, info);
}

/*
* Check if goods quantity is correct. Take struct info, argc and argv as
* arguments. If goods are correct, store them, else, exit program
*
* Retrun nothing
*/
void goods_quantity_check(int argc, const char *argv[], Info *info)
{
    int goodsQty;
    int goodsName = 2;
    int i = 0;
    int len;
    //check goods name and quantity. if valid, store it
    for (goodsQty = 3; goodsQty < argc; goodsQty += 2) {
        if(!valid_number(argv[goodsQty])) {
            free_allocated_space(argc - 2, info);
            exit_program(INVALID_QUANTITY);
        } else if(!valid_names((char *)argv[goodsName])) {
            free_allocated_space(argc - 2, info);
            exit_program(INVALID_NAME);
        } else {
            strcpy(info->goods[i], argv[goodsName]);
            len = strlen(info->goods[i]);
            info->goods[i][len] = '\0';
            info->qty[i] = atol(argv[goodsQty]);
        }
        goodsName += 2;
        i++;
    }
}

/*
* Check for validity of goods quantity. Take arguments of string of
* goods quantity. Quantity must be in number and cannot be negative.
*
* Function will return False for non number quantity and True
* for correct quantity format
*/
bool valid_number(const char *goodsQty)
{
    if(atol(goodsQty) == 0) {
        return false;
    }
    for(int i = 0; i < strlen(goodsQty); i++) {
        if(!isdigit(goodsQty[i])) {
            return false;
        } else if(goodsQty[i] < 0) {
            return false;
        }
    }

    return true;
}

/*
* Check for valid goods name. Take arguments of string of goods name.
*
* Return True if name does not contain banned character and False otherwise
*/
bool valid_names(char *name)
{
    for(int i = 0; i < strlen(name); i++) {
        if(name[i] == ' ') {
            return false;
        } else if(name[i] == '\n') {
            return false;
        } else if(name[i] == '\r') {
            return false;
        } else if(name[i] == ':') {
            return false;
        }
    }

    return true;
}

/*
* Allocating space for variables to be used. Take arguments of number of item
* and struct Info
*
* Return nothing
*/
void allocate_space(int numOfItem, Info *info)
{
    int i;
    int numOfGoods = numOfItem / 2;
    info->numOfDefer = 1;

    info->depotName = (char *)malloc(LENGTH * sizeof(char));
    //storing the goods name and quantity
    info->goods = (char **)malloc(numOfGoods * sizeof(char *));
    for(i = 0; i < numOfGoods; i++) {
        info->goods[i] = (char *)malloc(LENGTH * sizeof(char));
    }
    info->qty = (int *)malloc(numOfGoods * sizeof(int));
    info->key = (int *)malloc(1 * sizeof(int));
    info->deferMes = (char **)malloc(info->numOfDefer * sizeof(char *));
    for(i = 0; i < info->numOfDefer; i++) {
        info->deferMes[i] = (char *)malloc(LENGTH * sizeof(char));
    }
    info->nbr = (char **)malloc(100 * sizeof(char *));
    for(i = 0; i < 100; i++) {
        info->nbr[i] = (char *)malloc(LENGTH * sizeof(char));
    }
    h.fd = (int *)malloc(100 * sizeof(int));
    for(i = 0; i < 100; i++) {
        h.fd[i] = -1;
    }
    h.depot = (Depot **)malloc(100 * sizeof(Depot *));
    for(i = 0; i < 100; i++) {
        h.depot[i] = (Depot *)malloc(sizeof(Depot));
    }
    h.conHist = (char **)malloc(100 * sizeof(char *));
    for(i = 0; i < 100; i++) {
        h.conHist[i] = (char *)malloc(LENGTH * sizeof(char));
    }
}

/*
* Free all allocated space used in variables. Take arguments of number
* of items and struct Info
*
* Return nothing
*/
void free_allocated_space(int numOfItem, Info *info)
{
    int i;
    int numOfGoods = numOfItem / 2;

    free(info->depotName);
    //free space for storing the goods name and qty
    for(i = 0; i < numOfGoods; i++) {
        free(info->goods[i]);
    }
    free(info->goods);
}

/*
* Setup all newtwork related variables and set up the address. Take struct
* Info as argument. Process will be aborted if problem happens
*
* Return -1 to indicate unsuccessful process such as error in binding or
* error in connecting port. If successful, socket file descriptor (sockfd)
* is returned.
*/
int setup_network(Info *info)
{
    struct addrinfo *ai = 0;
    struct addrinfo hints;
    int err, sockfd;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo("localhost", 0, &hints, &ai)) {
        perror("getaddrinfo error");
        freeaddrinfo(ai);
        exit(-1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    err = bind(sockfd, (struct sockaddr *)ai->ai_addr,
            sizeof(struct sockaddr));
    // Do not proceed if problem happens
    if(err) {
        return -1;
    }
    if(port_info(sockfd, info) == false) {
        return -1;
    }
    listen(sockfd, 10);
    return sockfd;
}

/*
* Setting up port and print up connection port info. Take struct info
* and socket file descriptor as arguments.
*
* return false if process unsuccessful and true if setting up port successful
*/
bool port_info(int sockfd, Info *info)
{
    struct sockaddr_in ad;
    memset(&ad, 0, sizeof(struct sockaddr_in));
    socklen_t len = sizeof(struct sockaddr_in);
    if (getsockname(sockfd, (struct sockaddr *)&ad, &len)) {
        return false;
    }
    h.portNum = ntohs(ad.sin_port);
    fprintf(stdout, "%u\n", h.portNum);
    fflush(stdout);
    return true;
}

/*
* Accept connection and start threading for every new connection.
* Take arguments of socket file desctiptor and struct Info.
*
* Return nothing
*/
void accept_connection(int sockfd, Info *info)
{
    int connFd;
    // track changes in file descriptor
    int fdTrack;
    //initialize 1 Neighbour and allocate for index 0;
    while(connFd = accept(sockfd, 0, 0), connFd >= 0) {
        info->connFd = connFd;
        if((fdTrack != connFd) || (info->connFd != fdTrack)) {
            start_threading(info);
        }
        fdTrack = connFd;
    }
}

/*
* Entry point for thread setup. Take arguments of struct Info
*
* return nothing
*/
void start_threading(Info *info)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&info->depot.tid, &attr, thread_entry, info);
}

/*
* Thread entry point. Take argument of void pointer.
*
* Return NULL after thread termination
*/
void *thread_entry(void *arg)
{
    Info *info = (Info *)arg;
    int connFd = info->connFd;
    info->depot.receive = fdopen(connFd, "r");
    int n = ++h.thrCtr;
    h.fd[n] = connFd;
    h.depot[n]->receive = info->depot.receive;
    send_im_mes(info, connFd);
    info->connFd = connFd;
    info->mes = (char *)malloc(LENGTH * sizeof(char));
    while(connFd >= 0) {
        // keep reading until stream is closed
        if(!stream_reading(info)) {
            break;
        }
        //sort goods
        sort_lexiographic_order(info, 1);
        //sort neighbour
        sort_lexiographic_order(info, 2);
        //Update for signal handler use
        h.nbr = info->nbr;
        h.goods = info->goods;
        h.qty = info->qty;
        h.numOfItems = info->numOfItems;
    }
    pthread_exit(NULL);
}

/*
* Read input from stream. Take argument of struct Info. Message will be
* processed starting here.
*
* Return false if file pointer reads EOF and true if not.
*/
bool stream_reading(Info *info)
{
    //Set message size to maximum LENGTH
    char mes[LENGTH];
    if(fgets(mes, LENGTH, info->depot.receive) == NULL) {
        free(info->mes);
        return false;
    }
    pthread_mutex_lock(&lock);
    h.now = info->depot.receive;
    strcpy(info->mes, mes);
    process_message(info);
    pthread_mutex_unlock(&lock);
    memset(info->mes, '\0', LENGTH);
    // return back to start
    fseek(info->depot.receive, 0, SEEK_SET);
    return true;
}

/*
* Hub for processing message. Take argument of struct Info.
*
* Return nothing
*/
void process_message(Info *info)
{
    int mesFmt = message_format(info);
    switch(mesFmt) {
        case WRONG_FORMAT:
            return;
            break;
        case CONNECT:
            if(!check_connection_hist(info->mes)) {
                return;
            }
            connect_depot(info);
            break;
        case IM:
            receive_im_message(info);
            break;
        case DELIVER:
            deliver(info);
            break;
        case WITHDRAW:
            withdraw(info);
            break;
        case TRANSFER:
            transfer(info);
            break;
        case DEFER:
            defer(info);
            break;
        case EXECUTE:
            execute(info);
            break;
    }
}

/*
* Updating signal handler variables. Take struct info as arguments
* to update the global variables
*
* Return nothing
*/
void update_signal_handler(Info *info)
{
    //Update for signal handler use
    h.nbr = info->nbr;
    h.goods = info->goods;
    h.qty = info->qty;
    h.numOfItems = info->numOfItems;
}

/*
* Break message format and check. Take argument of struct Info
*
* return MessageType enum
*/
MessageType message_format(Info *info)
{
    // iterating through the message character
    int i = 0;
    // message buffer
    char *buf = (char *)malloc(LENGTH * sizeof(char));
    // which message
    int mes;
    if(!correct_message_format(info->mes, info)) {
        return WRONG_FORMAT;
    }
    while(info->mes[i] != ':') {
        buf[i] = info->mes[i];
        i++;
    }
    buf[i] = '\0';
    mes = which_message(buf);
    free(buf);
    return mes;
}

/*
* Check message according to format. Take argument of buffer string which
* represent the message format
*
* return MessageType enum. Will also return other that defined message format
* to indicate worng format
*/
MessageType which_message(char *buf)
{
    if(strcmp(buf, "Connect") == 0) {
        return CONNECT;
    } else if(strcmp(buf, "IM") == 0) {
        return IM;
    } else if(strcmp(buf, "Deliver") == 0) {
        return DELIVER;
    } else if(strncmp(buf, "Withdraw", sizeof("Withdraw")) == 0) {
        return WITHDRAW;
    } else if(strcmp(buf, "Transfer") == 0) {
        return TRANSFER;
    } else if(strcmp(buf, "Defer") == 0) {
        return DEFER;
    } else if(strcmp(buf, "Execute") == 0) {
        return EXECUTE;
    } else {
        return WRONG_FORMAT;
    }
}

/*
* Making sure message is in correct format. Take arguments of struct Info and
* input message. This function check number of parameters of the message and
* overall format check
*
* Return true if format is correct and false otherwise
*/
bool correct_message_format(char *mes, Info *info)
{
    //iterate every char
    int i = 0;
    //count number of semicolon
    int semicolonCounter = 0;
    //store split message
    char split[6][LENGTH];
    char buf;
    //Position of char in split message array
    int row = 0, col = 0;
    while(i < strlen(mes)) {
        if(mes[i] == ':') {
            semicolonCounter += 1;
            split[row][col] = '\0';
            row++;
            col = 0;
        } else {
            split[row][col] = mes[i];
            col++;
        }
        i++;
    }
    buf = split[row][col - 1];
    end_check(buf) ? (split[row][col - 1] = '\0') : (split[row][col] = '\0');
    info->goodsStrLen = col;
    if(semicolonCounter < 1 || semicolonCounter > 5 ||
            !correct_parameters(split, info)) {
        return false;
    }
    if(!correct_message_arg(semicolonCounter, which_message(split[0]))) {
        return WRONG_FORMAT;
    }
    return true;
}

/*
* Perform checking for number of argumens for every messages. Take number of
* semicolon which is the number of arguments and message type which is
* the message format
*
* Return nothing
*/
bool correct_message_arg(int numOfSemColon, int mesType)
{
    if(((mesType == CONNECT) || (mesType == EXECUTE)) &&
            (numOfSemColon != 1)) {
        return false;
    } else if(((mesType == DELIVER) || (mesType == WITHDRAW) ||
            (mesType == IM)) && (numOfSemColon != 2)) {
        return false;
    } else if((mesType == TRANSFER) && (numOfSemColon != 3)) {
        return false;
    } else if(mesType == DEFER) {
        if((numOfSemColon < 4) || (numOfSemColon > 5)) {
            return false;
        }
    }
    return true;
}

/*
* Check buffer end for carraige return and new line. Take single char as
* arguments.
*
* Return true if found, false otherwise
*/
bool end_check(char buf)
{
    if((buf == '\r') || (buf == '\n')) {
        return true;
    }
    return false;
}

/*
* Entry point for checking correct parameters. Take 2d array which is
* is the arguments of messages and struct Info.
*
* Return false if incorrect format and true if correct
*/
bool correct_parameters(char split[][LENGTH], Info *info)
{
    switch(which_message(split[0])) {
        case CONNECT:
            if(atol(split[1]) < 1 || atol(split[1]) > 65535) {
                return false;
            }
            break;
        case IM:
            return im_check(split);
            break;
        case TRANSFER:
            return transfer_check(split);
            break;
        case DEFER:
            return defer_check(split, info);
            break;
        case EXECUTE:
            if(atol(split[1]) < 0) {
                return false;
            }
            break;
        case WRONG_FORMAT:
            return false;
            break;
        default:
            //to handle DELIVER and WITHDRAW
            return deliver_withdraw_check(split);
    }
    return true;
}

/*
* Format check for DELIVER and WITHDRAW message. Take 2d array as argument
* which represent message parameters.
*
* Return true if correct format and false otherwise
*/
bool deliver_withdraw_check(char split[][LENGTH])
{
    if(split[2][strlen(split[2]) - 1] == '\r') {
        split[2][strlen(split[2]) - 1] = '\0';
    }
    if(!valid_number(split[1])) {
        return false;
    } else if(!valid_names(split[2])) {
        return false;
    }
    return true;
}

/*
* Check if newly connect message has been sent before. Prevent multiple
* connection. Take arguments of string which is CONNECT message
*
* return true if no connection history and false otherwise
*/
bool check_connection_hist(char *newCon)
{
    for(int i = 0; i < h.numOfCon; i++) {
        if(strcmp(h.conHist[i], newCon) == 0) {
            return false;
        }
    }
    // store connection messages if new
    strcpy(h.conHist[h.numOfCon++], newCon);
    return true;
}

/*
* Check intrduction message, IM. Take 2d array as input which is the message
* parameter.
*
* Return true if correct format and false otherwise
*/
bool im_check(char split[][LENGTH])
{
    if(atol(split[1]) < 1 || atol(split[1]) > 65535) {
        return false;
    } else if(!valid_names(split[2])) {
        return false;
    }
    return true;
}

/*
* Check transfer message. Take arguments of 2d array which is the
* arguments of messages.
*
* Return true if correct format and false otherwise
*/
bool transfer_check(char split[][LENGTH])
{
    if(!valid_number(split[1])) {
        return false;
    } else if(!valid_names(split[2]) || !valid_names(split[3])) {
        return false;
    }
    return true;
}

/*
* Check defer message. Take arguments of 2d array which is the arguments
* of messages.
*
* Return true if corret format and false otherwise
*/
bool defer_check(char split[][LENGTH], Info *info)
{
    char deferMes[3][LENGTH];
    if(which_message(split[2]) != TRANSFER) {
        strcpy(deferMes[0], split[2]);
        strcpy(deferMes[1], split[3]);
        strcpy(deferMes[2], split[4]);
    } else {
        strcpy(deferMes[0], split[2]);
        strcpy(deferMes[1], split[3]);
        strcpy(deferMes[2], split[4]);
        strcpy(deferMes[4], split[5]);
    }
    // perform recursion to check for deffered message
    if((atol(split[1]) < 0) || !correct_parameters(deferMes, info)) {
        return false;
    }
    return true;
}
