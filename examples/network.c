#include <banjo/log.h>
#include <banjo/net.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    struct bj_tcp_listener* listener = bj_bind(0, 8080, 5);

    bj_trace("%p", listener);

    bj_unbind(listener);




    return 0;
}
