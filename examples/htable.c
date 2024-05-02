#include <assert.h>

#include <banjo/htable.h>

int main()
{
    // Create a new hash table
    bj_htable* table = bj_new(htable, default, sizeof(int), sizeof(int)); // No custom allocator
    assert(table); // Ensure table creation was successful

    // Insert values into the table
    int key1 = 123;
    int value1 = 456;
    int key2 = 789;
    int value2 = 101112;
    bj_htable_set(table, &key1, &value1);
    bj_htable_set(table, &key2, &value2);

    // Retrieve values from the table
    int* retrieved_value1 = (int*)bj_htable_get(table, &key1, 0);
    assert(retrieved_value1 && *retrieved_value1 == value1); // Ensure value retrieval was successful
    int* retrieved_value2 = (int*)bj_htable_get(table, &key2, 0);
    assert(retrieved_value2 && *retrieved_value2 == value2); // Ensure value retrieval was successful

    // Clear the table
    bj_htable_clear(table);

    // Reset the table
    bj_htable_reset(table);

    // Delete the table
    bj_del(htable, table);

    return 0;
}
