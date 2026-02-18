#include <banjo/log.h>
#include <banjo/net.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    struct bj_error* error = 0;
    struct bj_tcp_listener* listener = bj_listen_tcp(0, 8080, &error);
    if(!listener) {
        bj_err("bind failed (%s)", bj_error_message(error));
        bj_clear_error(&error);
        return 1;
    }

    struct bj_tcp_stream* stream = bj_accept_tcp(listener, &error);
    if(stream) {

        char buf[1024];
        int n = bj_tcp_recv(stream, buf, sizeof(buf));
        if(n > 0) {
            bj_tcp_send(stream, buf, n); // echo back
        }

        bj_close_tcp_stream(stream);
    }

    bj_close_tcp_listener(listener);

    return 0;
}
