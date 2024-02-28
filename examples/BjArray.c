#include <banjo/array.h>

int main(int argc, char* argv[]) {

    BjArray* array = bj_array_new(&(BjArrayInfo) {
        .value_size = sizeof(int),
    }, 0);

    int a = 42;
    bj_array_push(array, &a);

    bj_assert(bj_array_count(array) == 1);
}
