#include <banjo/log.h>
#include <banjo/net.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    struct bj_net_address_info* list = bj_get_local_addresses();

    char buf[128];
    for (struct bj_net_address_info* node = list; node; node = node->next) {
        bj_sprint_address(node->address, buf, sizeof(buf));
        bj_info("%s", buf);
    }

    bj_free_address_info(list);

    return 0;
}
