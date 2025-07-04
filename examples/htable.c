////////////////////////////////////////////////////////////////////////////////
/// \example htable.c
/// Hash Table example.
////////////////////////////////////////////////////////////////////////////////

#include <banjo/htable.h>
#include <banjo/main.h>
#include <banjo/log.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Create a new hash table
    bj_htable* table = bj_htable_new_t(int, int); // No custom allocator

    // Insert values into the table
    int key1   = 123;
    int value1 = 456;
    int key2   = 789;
    int value2 = 101112;
    bj_htable_set(table, &key1, &value1);
    bj_htable_set(table, &key2, &value2);

    // Retrieve values from the table
    int* retrieved_value1 = (int*)bj_htable_get(table, &key1, 0);
    bj_info("{%d} = %d", key1, *retrieved_value1);
    int* retrieved_value2 = (int*)bj_htable_get(table, &key2, 0);
    bj_info("{%d} = %d", key2, *retrieved_value2);

    // Delete the table
    bj_htable_del(table);

    return 0;
}
