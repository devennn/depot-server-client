#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "depot.h"

// For error handling parameters
Handle h;

// Sighup Handler.
void sighup_handler(int signum)
{
    int i;
    printf("Goods:\n");
    for(i = 0; i < h.numOfItems; i++) {
        if(h.qty[i] == 0) {
            continue;
        }
        printf("%s %d\n", h.goods[i], h.qty[i]);
    }
    printf("Neighbours:\n");
    //start storing at index 0. Offset 2 is needed
    for(i = 0; i < h.numOfNbr - 1; i++) {
        printf("%s\n", h.nbr[i]);
    }
    fflush(stdout);
}

// Setting up sigaction
void catch_signal(int signum)
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    action.sa_handler = sighup_handler;
    sigaction(signum, &action, NULL);
}

int main(int argc, char const *argv[])
{
    Info *info = malloc(sizeof(Info));
    int sockfd;
    catch_signal(SIGHUP);
    signal(SIGPIPE, SIG_IGN);

    // Initialize counter
    h.numOfNbr = 1;
    h.thrCtr = -1;
    h.numOfCon = 0;
    allocate_space(argc - 2, info);
    parse_args(argc, argv, info);

    // Initializing global struct for signal handler
    h.nbr = info->nbr;
    h.goods = info->goods;
    h.qty = info->qty;
    h.numOfItems = info->numOfItems;

    sockfd = setup_network(info);
    accept_connection(sockfd, info);

    return 0;
}
