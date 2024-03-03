#include <banjo/array.h>

int main(int argc, char* argv[]) {

    /*BjArray* array = bj_array_new(0, 
        &(BjArrayInfo) {
        .bytes_payload = sizeof(int),
    });*/

    

    BjArrayInfo info = { .bytes_payload = sizeof(int) };
    BjArray* array = bj_array_new(&info, 0);

    int a = 42;
    bj_array_push(array, &a);

    bj_assert(bj_array_count(array) == 1);

    bj_array_del(array);
}
