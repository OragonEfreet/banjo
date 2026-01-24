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

    printf("Configuring local address...\n");
    struct addrinfo hints = {
        .ai_family   = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_flags    = AI_PASSIVE,
    };

    struct addrinfo* bind_address = 0;
    bj_assert(getaddrinfo(0, "8081", &hints, &bind_address) == 0);

    printf("Creating socket...\n");

    SOCKET socket_listen = socket(
        bind_address->ai_family,
        bind_address->ai_socktype,
        bind_address->ai_protocol
    );

    if(!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Binding socket to local address...\n");
    if(bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);

    printf("Listening...\n");
    if(listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(
        socket_listen, 
        (struct sockaddr*) &client_address,
        &client_len
    );
    if(!ISVALIDSOCKET(socket_client)) {
        fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Client is connected... ");
    char address_buffer[100];
    getnameinfo((struct sockaddr*)&client_address,
        client_len,
        address_buffer,
        sizeof(address_buffer),
        0, 0, NI_NUMERICHOST
    );
    printf("%s\n", address_buffer);



    return 0;
}
