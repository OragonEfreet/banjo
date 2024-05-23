#include <banjo/error.h>
#include <banjo/log.h>

#define CODE 101

/// [Return Errors]
void function_returning_error(bj_error** error) {
    bj_set_error(error, CODE, "An error occured");
}
/// [Return Errors]

void function_calling_failing_function(bj_error** error) {
    bj_error* sub_err = 0;

    function_returning_error(&sub_err);

    if(sub_err != 0) {
        bj_forward_error(sub_err, error);
        return;
    }

    bj_info("This should not be printed\n");
}


int main(int argc, char* argv[]) {

    // If you are not interested by an error, pass 0 to the bj_error* pointer:
    function_returning_error(0);

    /// [Using bj_error]
    bj_error* error = 0;

    function_returning_error(&error);

    if(error != 0) {
        bj_info("There was an error");
    }
    /// [Using bj_error]
    
    if (bj_error_check(error, CODE)) {
        bj_info("Error domain and code match");
    }

    bj_clear_error(&error);

    function_calling_failing_function(&error);

    if (bj_error_check(error, CODE)) {
        bj_info("Error from nested function");
    }

    return 0;
}
