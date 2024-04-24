#include <assert.h>

#include <banjo/hash_table.h>

int main()
{
    // Create a hash table information object
    BjHashTableInfo table_info = {
        .bytes_value = sizeof(int), // Size of each value in the table
        .bytes_key = sizeof(int), // Size of each key in the table
        .weak_owning = 0, // Strong ownership
        .fn_hash = 0 // Placeholder hash function (not provided in the header)
    };

    // Create a new hash table
    BjHashTable* table = bj_hash_table_new(&table_info); // No custom allocator
    assert(table); // Ensure table creation was successful

    // Insert values into the table
    int key1 = 123;
    int value1 = 456;
    int key2 = 789;
    int value2 = 101112;
    bj_hash_table_set(table, &key1, &value1);
    bj_hash_table_set(table, &key2, &value2);

    // Retrieve values from the table
    int* retrieved_value1 = (int*)bj_hash_table_get(table, &key1, 0);
    assert(retrieved_value1 && *retrieved_value1 == value1); // Ensure value retrieval was successful
    int* retrieved_value2 = (int*)bj_hash_table_get(table, &key2, 0);
    assert(retrieved_value2 && *retrieved_value2 == value2); // Ensure value retrieval was successful

    // Clear the table
    bj_hash_table_clear(table);

    // Reset the table
    bj_hash_table_reset(table);

    // Delete the table
    bj_hash_table_del(table);

    return 0;
}
