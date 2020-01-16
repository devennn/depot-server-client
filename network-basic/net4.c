//gcc net4.c
//./a.out


#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    struct addrinfo* ai = 0;
    struct addrinfo hints;
    memset(& hints, 0, sizeof(struct addrinfo));
    hints.ai_family=AF_INET;        // IPv4  for generic could use AF_UNSPEC
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;  // Because we want to bind with it
    int err;

    /*
    * Network address and service translation
    */
    if (err=getaddrinfo("127.0.0.1", 0, &hints, &ai)) { // no particular port
        freeaddrinfo(ai);
        fprintf(stderr, "%s\n", gai_strerror(err));
        return 1;   // could not work out the address
    }

    /*
    * create and endpoint for communication
    * On success, it will return lowest number file descriptor which is
    * not used. Will never return 0,1,2 since this is default value of
    * stdin, stdout and stderr. Will return -1 if fail
    */
    int socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0); // 0 == use default protocol
    printf("socketFileDescriptor -> %d\n", socketFileDescriptor);

    /*
    * Bind name to a socket.
    * When, socket is created with socket(), the file descriptor which represent
    * the socket exists in a name space (address family). But it has no address
    * assign to it. Bind() assigns address to the socket represent by file descriptor.
    */
    if (bind(socketFileDescriptor, (struct sockaddr*)ai->ai_addr, sizeof(struct sockaddr))) {
        perror("Binding");
        return 3;
    }

    /*
    * Get socket name
    */
    struct sockaddr_in ad;
    memset(&ad, 0, sizeof(struct sockaddr_in));
    socklen_t len=sizeof(struct sockaddr_in);
    if (getsockname(socketFileDescriptor, (struct sockaddr*)&ad, &len)) {
        perror("sockname");
        return 4;
    }

    // converts the unsigned short integer netshort from network byte order to host byte order.
    printf("Port -> %u\n", ntohs(ad.sin_port));
    printf("Address Fam -> %u\n", ntohs(ad.sin_family));

    /*
    * listen for connections on a socket
    * Marks the socket refered by socket file descriptor as passive. This socket
    * is used to accept incoming connection requests using accept()
    */
    if (listen(socketFileDescriptor, 10)) {     // allow up to 10 connection requests to queue
        perror("Listen");
        return 4;
    }

    /*
    * accept a connection on a socket
    * It extracts the first connection request on the queue of pending
    * connections for the listening socket, sockfd, creates a new connected
    * socket, and returns a new file descriptor referring to that socket.
    */
    int conn_fd;
    char msg[10];
    // change 0, 0 to get info about other end
    if (conn_fd = accept(socketFileDescriptor, 0, 0), conn_fd >= 0) {
        FILE* stream = fdopen(conn_fd, "r");
        while(1) {
            fgets(msg, 10, stream);
            fputs(msg, stdout);
            fflush(stdout);
        }
        fclose(stream);
    }
    return 0;
}
