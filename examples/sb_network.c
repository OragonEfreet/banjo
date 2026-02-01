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

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    printf("Configuring local address\n");
    const struct addrinfo hints = {
        .ai_family   = AF_INET6,
        .ai_socktype = SOCK_STREAM,
        .ai_flags    = AI_PASSIVE,
    };

    struct addrinfo *bind_address;

    // After several times I used this, I still don't understand what this is doing.
    // I mean yes I understand, it's searching for an avaiable local socket.
    // But why a LOCAL one ? A server is listening to possibly tons of connexion, how are
    // they all "one socket" ?
    // Or maybe I'm making some confusion between socket, connexion, address and interface?
    const int res = getaddrinfo(0, "8080", &hints, &bind_address);
    if(res) {
        fprintf(stderr, "cannot configure local address: %s\n", gai_strerror(res));
        return 1;
    }

    // Another thing... so we create a socket for listening, and my socket is a port 8080.
    // Is a socket a port then? Also do the client who connects to me know the port they have to send to?
    // I really feel like there are 2 sockets in a connexion, and I'm not talking about the two computers,
    // but that in a connexion, my server has 2 sockets, one representing "me" and one representing the client, that I
    // get from the accept() function, am I right? In this case what is the port of the client is a socket is a port ?
    printf("Creating the socket...\n");
    const int socket_listen = socket(
        bind_address->ai_family,
        bind_address->ai_socktype,
        bind_address->ai_protocol
    );
    if(socket_listen <= 0)  {
        fprintf(stderr, "socket() failed. (%d)\n", errno);
        return 1;
    }

    int v6only = 0;
    setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only));

    printf("Binding socket to local address...\n");
    if(bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", errno);
        return 1;
    }
    freeaddrinfo(bind_address);

    printf("Listening...\n");
    if(listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", errno);
        return 1;
    }

    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    const int socket_client = accept(
        socket_listen,
        (struct sockaddr*)&client_address,
        &client_len
    );
    if(socket_client <= 0) {
        fprintf(stderr, "accept() failed. (%d)\n", errno);
        return 1;
    }

    printf("Client is connected... ");
    char address_buffer[100];
    getnameinfo(
        (struct sockaddr*)&client_address, client_len, 
        address_buffer, sizeof(address_buffer),
        0, 0,
        NI_NUMERICHOST
    );
    printf("%s\n", address_buffer);

    // When reading the request, how do I know my client finished sending everything?
    // is there a risk I don't listen to everything? 
    // How can i make the difference between a message and the next one if I can read splittily a single message?
    // Or does it mean that one message == tcp connexion ?
    printf("Reading request...\n");
    char request[1024];
    int bytes_received = recv(socket_client, request, 1024, 0);
    printf("Received %d bytes.\n", bytes_received);
    printf("%.*s.\n", bytes_received, request);

    printf("Sending response...\n");
    const char* response = 
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Local time is: ";
    int bytes_sent = send(socket_client, response, strlen(response), 0);
    printf("Send %d of %d bytes.\n", bytes_sent, (int)strlen(response));

    time_t timer;
    time(&timer);
    char *time_msg = ctime(&timer);
    bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
    printf("Send %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

    printf("Closing connection...\n");
    close(socket_client);

    printf("Closing listening socket...\n");
    close(socket_listen);

    printf("Finished\n");

    return 0;
}
