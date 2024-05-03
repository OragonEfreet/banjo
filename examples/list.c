#include <assert.h>
#include <banjo/list.h>

int main() {

    // Create a new list
    bj_list* list = bj_new(list, default, int); // No custom allocator
    assert(list); // Ensure list creation was successful

    // Insert elements into the list
    bj_list_insert(list, 0, &(int){10});
    bj_list_insert(list, 1, &(int){20});
    bj_list_insert(list, 2, &(int){30});

    // Get the number of elements in the list
    assert(bj_list_len(list) == 3); // Ensure the count is correct

    // Accessing elements by index
    assert(*(int*)bj_list_at(list, 0) == 10); // Ensure the value at index 0 is correct
    assert(*(int*)bj_list_at(list, 1) == 20); // Ensure the value at index 1 is correct
    assert(*(int*)bj_list_at(list, 2) == 30); // Ensure the value at index 2 is correct

    // Iterating over the list using an iterator
    bj_list_iterator* iterator = bj_list_iterator_new(list);
    assert(iterator); // Ensure iterator creation was successful
    while (bj_list_iterator_has_next(iterator)) {
        bj_list_iterator_next(iterator);
    }
    bj_list_iterator_del(iterator);

    // Clearing the list
    bj_list_clear(list);
    assert(bj_list_len(list) == 0); // Ensure the list is cleared

    // Resetting the list
    bj_list_reset(list);

    // Deleting the list
    bj_del(list, list);

    return 0;
}
