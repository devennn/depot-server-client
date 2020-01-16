#include <netdb.h>
#include <string.h>
#include <stdio.h>

void print_quads(in_addr_t x) {
    char* bytes = (char*)&x;  
    for (int i=0; i<sizeof(x); ++i) {
        printf("%u.", (unsigned)bytes[i]);
    }
}

int main(int argc, char** argv) {
    if (argc<2) {
        fprintf(stderr, "Not enough args\n");
        return 2;
    }
    const char* port=argv[1];
    struct addrinfo* ai = 0;
    struct addrinfo hints;
    memset(& hints, 0, sizeof(struct addrinfo));
    hints.ai_family=AF_INET;        // IPv6  for generic could use AF_UNSPEC
    hints.ai_socktype=SOCK_STREAM;
    int err;
    if (err=getaddrinfo("localhost", port, &hints, &ai)) {
        freeaddrinfo(ai);
        fprintf(stderr, "%s\n", gai_strerror(err));
        return 1;   // could not work out the address
    }
    // now let's try to get the IP address out
    struct sockaddr* sa=ai->ai_addr;
    struct sockaddr_in* sai=(struct sockaddr_in*)sa;
    in_addr_t address = sai->sin_addr.s_addr;

    print_quads(address);   // does not consider endianness
                            // inet_ntoa() would be better
    return 0;
}
