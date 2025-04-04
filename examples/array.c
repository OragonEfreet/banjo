////////////////////////////////////////////////////////////////////////////////
/// \example array.c
/// Demonstrates how to use the \ref bj_array container, the vector-like structure.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/api.h>
#include <banjo/array.h>
#include <banjo/memory.h>
#include <assert.h>

int main() {

    bj_array* array = bj_array_new_t(int);

    // Add a new element in the array
    int a = 42;
    bj_array_push(array, &a);

    // Get array length
    size_t len = bj_array_len(array);
    assert(len == 1);

    // Remove the last element from the array
    bj_array_pop(array);

    // Clear array
    bj_array_clear(array);

    // Clearing the array doesn't change the array memory usage
    size_t capacity = bj_array_capacity(array);
    assert(capacity >= 1);

    // It's possible to reallocate the array so that it only uses the necessary
    // space in memory:
    bj_array_shrink(array);

    capacity = bj_array_capacity(array);
    assert(capacity == 0);

    // Use bj_array_at() to retrieve a value at the given index
    int val_0 = 17;
    int val_1 = -50;
    int val_2 = 101;

    bj_array_push(array, &val_0);
    bj_array_push(array, &val_1);
    bj_array_push(array, &val_2);

    int* val = bj_array_at(array, 0); assert(*val == val_0);
         val = bj_array_at(array, 1); assert(*val == val_1);
         val = bj_array_at(array, 2); assert(*val == val_2);

    // You can directly access the array underlying data
    int* data = bj_array_data(array);
    assert(*data++ == val_0);
    assert(*data++ == val_1);
    assert(*data++ == val_2);

    bj_array_del(array);
}
