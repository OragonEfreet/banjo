#include <stdio.h>

#include <string.h>
#include <stdlib.h>

#define HASH_TABLE_SIZE 10000

typedef struct entry_t {
    char* key;
    char* value;
    struct entry_t* next;
} entry_t;

typedef struct {
    entry_t** entries;
} ht_t;

ht_t* ht_new(void) {
    // ajllocate table
    ht_t* hashtable = malloc(sizeof(ht_t));

    // allocate table entries
    hashtable->entries = malloc(sizeof(entry_t*) * HASH_TABLE_SIZE);
    
    // Set each to null, needed
    for(int i = 0 ; i < HASH_TABLE_SIZE ; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}


unsigned int hash(const char* key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);
    // do several rounds of multiplication
    for(; i < key_len ; ++i) {
        value = value * 37 + key[i];
    }
    // make sure value is 0 <= value < TABLE_SIZE
    value = value % HASH_TABLE_SIZE;
    return value;
}


entry_t* ht_pair(const char* key, const char* value) {
    // allocate the entry
    entry_t* entry = malloc(sizeof(entry_t));
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) + 1);

    // copy keys and values
    strcpy(entry->key, key);
    strcpy(entry->value, value);

    entry->next = NULL;

    return entry;
}

void ht_set(ht_t* hashtable, const char* key, const char* value) {
    unsigned int slot = hash(key);

    // Try to look up an entry set
    entry_t* entry = hashtable->entries[slot];

    // no entry means slot empty, isnert immediately
    if (entry == 0) {
        hashtable->entries[slot] = ht_pair(key, value);
        return;
    }

    entry_t* prev = 0;

    // walk through each entry until either the end is reached or a matching key
    // is found.
    while(entry != 0) {
        if(strcmp(entry->key, key) ==0) {
            // match found, replace value
            free(entry->value);
            entry->value = malloc(strlen(value) + 1);
            strcpy(entry->value, value);
            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;
    }

    // end of chain reached without a match, add the value
    prev->next = ht_pair(key, value);
}


char* ht_get(ht_t* hashtable, const char* key) {
    unsigned int slot = hash(key);


    // try to find a valid slot
    entry_t* entry = hashtable->entries[slot];

    // no slot means no entry
    if(entry == 0) {
        return 0;
    }


    while(entry != 0) {

        if(strcmp(entry->key, key) == 0) {
            return entry->value;
        }

        entry = entry->next;
    }


    return 0;
}


void ht_dump(ht_t* hashtable) {
    for(int i = 0 ; i < HASH_TABLE_SIZE ; ++i) {
        entry_t* entry = hashtable->entries[i];
        if(entry == NULL) {
            continue;
        }

        printf("slot[%4d]: ", i);


        for(;;) {
            printf("%s=%s ", entry->key, entry->value);

            if(entry->next == 0) {
                break;
            }

            entry = entry->next;
        }
        printf("\n");
    }
}


int main(int rgc, char* arg[]) {

    ht_t *ht = ht_new();


    ht_set(ht, "name1", "em");
    ht_set(ht, "name2", "russian");
    ht_set(ht, "name3", "pizza");
    ht_set(ht, "name4", "doge");
    ht_set(ht, "name5", "pyro");
    ht_set(ht, "name6", "joost");
    ht_set(ht, "name7", "kalix");

    ht_dump(ht);

    return 0;
}
