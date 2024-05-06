#include <banjo/error.h>
#include <banjo/log.h>

#define DOMAIN 42
#define CODE 101

/// [Return Errors]
void function_returning_error(bj_error* error) {
    bj_set_error(error, DOMAIN, CODE);
}
/// [Return Errors]

int main(int argc, char* argv[]) {

    /// [Using bj_error]
    bj_error error;

    function_returning_error(&error);

    if(error) {
        bj_error("Error!");
    }
    /// [Using bj_error]

    return 0;
}
