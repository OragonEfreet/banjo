#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <banjo/assert.h>

#define SOCKET int
#define ISVALIDSOCKET(s) ((s) > 0)
#define CLOSESOCKET(s) close(s)
#define GETSOCKETERRNO() (errno)

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;


    printf("Configuring local address\n");
    const struct addrinfo hints = {
        .ai_family   = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_flags    = AI_PASSIVE,
    };

    struct addrinfo *bind_address;
    const int res = getaddrinfo(0, "8080", &hints, &bind_address);
    if(res) {
        fprintf(stderr, "cannot configure local address: %s\n", gai_strerror(res));
        return 1;
    }


    


    return 0;
}
