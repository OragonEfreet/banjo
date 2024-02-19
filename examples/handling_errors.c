#include <banjo/error.h>
#include <banjo/log.h>

#define DOMAIN 42
#define CODE 101

/// [Return Errors]
void function_returning_error(BjError* error) {
    bj_set_error(error, DOMAIN, CODE);
}
/// [Return Errors]

int main(int argc, char* argv[]) {

    /// [Using BjError]
    BjError error;

    function_returning_error(&error);

    if(error.code) {
        bj_error("Error!");
    }
    /// [Using BjError]

    return 0;
}
