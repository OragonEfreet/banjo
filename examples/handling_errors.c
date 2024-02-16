#include <banjo/error.h>
#include <banjo/log.h>

#define DOMAIN 42
#define CODE 101

void function_returning_error(BjError* error) {
    bjSetError(error, DOMAIN, CODE);
}

int main(int argc, char* argv[]) {

    BjError error;

    function_returning_error(&error);

    if(error.code) {
        bjLogWarn("Error!");
    }

    return 0;
}
