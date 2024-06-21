#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/window.h>

int main(int argc, char* argv[]) {

    bj_error* p_error = 0;

    if(!bj_system_init(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return 1;
    } 

    bj_window* window = bj_window_new("Simple Banjo Window", 100, 100, 800, 600);

    bj_window_del(window);

    bj_system_dispose(0);
    return 0;
}
