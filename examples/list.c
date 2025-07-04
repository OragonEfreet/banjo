////////////////////////////////////////////////////////////////////////////////
/// \example list.c
/// Single chained list example.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/assert.h>
#include <banjo/main.h>
#include <banjo/list.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Create a new list
    bj_list* list = bj_list_new_t(int); // No custom allocator
    bj_assert(list); // Ensure list creation was successful

    // Insert elements into the list
    bj_list_insert(list, 0, &(int){10});
    bj_list_insert(list, 1, &(int){20});
    bj_list_insert(list, 2, &(int){30});

    // Get the number of elements in the list
    bj_assert(bj_list_len(list) == 3); // Ensure the count is correct

    // Accessing elements by index
    bj_assert(*(int*)bj_list_at(list, 0) == 10); // Ensure the value at index 0 is correct
    bj_assert(*(int*)bj_list_at(list, 1) == 20); // Ensure the value at index 1 is correct
    bj_assert(*(int*)bj_list_at(list, 2) == 30); // Ensure the value at index 2 is correct

    // Iterating over the list using an iterator
    bj_list_iterator* iterator = bj_list_iterator_new(list);
    bj_assert(iterator); // Ensure iterator creation was successful
    while (bj_list_iterator_has_next(iterator)) {
        bj_list_iterator_next(iterator);
    }
    bj_list_iterator_del(iterator);

    // Clearing the list
    bj_list_clear(list);
    bj_assert(bj_list_len(list) == 0); // Ensure the list is cleared

    // Deleting the list
    bj_list_del(list);

    return 0;
}
