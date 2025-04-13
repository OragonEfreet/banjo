////////////////////////////////////////////////////////////////////////////////
/// \example array.c
/// Demonstrates how to use the \ref bj_array container, the vector-like structure.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/api.h>
#include <banjo/array.h>
#include <banjo/log.h>
#include <banjo/memory.h>

int main(void) {

    bj_array* array = bj_array_new_t(int);

    // Add a new element in the array
    int a = 42;
    bj_array_push(array, &a);

    // Get array length
    size_t len = bj_array_len(array);
    bj_info("Array length: %d\n", len);

    // Remove the last element from the array
    bj_array_pop(array);

    // Clear array
    bj_array_clear(array);

    // Clearing the array doesn't change the array memory usage
    size_t capacity = bj_array_capacity(array);
    bj_info("Capacity: %d", capacity);

    // It's possible to reallocate the array so that it only uses the necessary
    // space in memory:
    bj_array_shrink(array);

    capacity = bj_array_capacity(array);
    bj_info("Capacity after shrink: %d", capacity);

    // Use bj_array_at() to retrieve a value at the given index
    int val_0 = 17;
    int val_1 = -50;
    int val_2 = 101;

    bj_array_push(array, &val_0);
    bj_array_push(array, &val_1);
    bj_array_push(array, &val_2);

    int* val = bj_array_at(array, 0); bj_info(".at(0) = %d", *val);
         val = bj_array_at(array, 1); bj_info(".at(1) = %d", *val);
         val = bj_array_at(array, 2); bj_info(".at(2) = %d", *val);

    // You can directly access the array underlying data
    int* data = bj_array_data(array);
    bj_info("data = [%d, %d, %d]", data[0], data[1], data[2]);

    bj_array_del(array);
}
