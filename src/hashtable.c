#include <stdlib.h>     // for calloc and free
#include <string.h>
#include "hashtable.h"

// global constants
const static int INITIAL_CAPACITY = 8;
const static double GROWTH_THRESHHOLD = 0.7;
const static hkey_t bucket_empty = NULL;
const hvalue_t not_in_table = NULL;

/**
 * A C style constructor for Node
 * Parameters:
 *      key - The key for this node
 * Returns:
 *      The pointer to the new Node object
 */
Entry_t *Entry_create(hkey_t key, hvalue_t value) {
    Entry_t *node = malloc(sizeof(Entry_t));
    
    if (node != NULL) {
        node->key = key;
        node->value = value;
    }

    return node;
}

/**
 * A C style destuctor for Node
 * Parameters:
 *      node - The pointer to the Node to destroy
 */
void Entry_destroy(Entry_t *node) {
    free(node);
}

/**
 * A C style contructor for HashTable
 * Returns:
 *      A pointer to the new HashTable
 */
HashTable_t *HashTable_create() {
    HashTable_t *hashtable = malloc(sizeof(HashTable_t));
    
    if (hashtable != NULL) {
        hashtable->num_keys = 0;
        hashtable->num_buckets = INITIAL_CAPACITY;
        
        // zero initialized to mark all buckets as empty
        // NULL values denote an empty bucket
        hashtable->table = calloc(sizeof(Entry_t), hashtable->num_buckets);
        if (hashtable->table == NULL) {
            free(hashtable);
            return NULL;
        }
    }

    return hashtable;
}

/**
 * A C style destructor for HashTable
 * Parameters:
 *      hashtable - The HashTable to be destroyed
 */
void HashTable_destroy(HashTable_t *hashtable) {
    free(hashtable->table);
    free(hashtable);
}

/**
 * Returns the hash of a given key
 * Uses DJB2 hashing algorithm provided with assignment spec
 * Parameters:
 *      key - The key to hash
 *      num_buckets - The number of buckets in the table
 */
hhash_t HashTable_hash(hkey_t key, size_t num_buckets) {
    hhash_t hash = 5381;
    for (size_t c = *key; *key != 0; key++) {
        hash = hash * 33 + c;
    }

    // TODO change to bitmask
    return hash & (num_buckets - 1);
}

/**
 * Find the index of the bucket that contains the key
 * or the index of the next open bucket using linear probing
 * Parameters:
 *      hashtable - The HashTable to search
 *      key - The key to search for
 * Returns:
 *      The index of the bucket with key or empty bucket
 */
hhash_t HashTable_lookup(HashTable_t *hashtable, hkey_t key) {
    hhash_t index = HashTable_hash(key, hashtable->num_buckets);

    // find key or first empty spot using linear probing
    Entry_t *table = hashtable->table;
    while (table[index].key != NULL && strcmp(table[index].key, key) != 0) {
        // The HashTable is always grown before its filled
        // so its safe to assume there will always be an open spot
        index = ++index % hashtable->num_buckets;
    }

    return index;
}

/**
 * Internal version of insert.
 * Used by HashTable_insert and HashTable_grow
 * Does not grow the HashTable
 */
void HashTable_insert_internal(HashTable_t *hashtable, hkey_t key, hvalue_t value) {
    size_t index = HashTable_lookup(hashtable, key);
    Entry_t *entry = &hashtable->table[index];
    
    // if bucket was empty, insert key
    if (entry->key == bucket_empty) {
        ++hashtable->num_keys;
        entry->key = key;
    }

    // insert value
    entry->value = value;
}

/**
 * Grows the HashTable by a factor of 2
 * Uses when the load factor exceeds a certain threshhold
 * Parameters:
 *      hashtable - The HashTable to grow
 */
void HashTable_grow(HashTable_t *hashtable) {
    // preserve old array
    size_t previous_size = hashtable->num_buckets;
    Entry_t *old_table = hashtable->table;

    // allocate new array
    hashtable->num_buckets *= 2;
    hashtable->table = calloc(sizeof(Entry_t), hashtable->num_buckets);

    // copy entries from the old table to the new table
    for (size_t i = 0; i < previous_size; i++) {
        if (old_table[i].key != bucket_empty) {
            Entry_t entry = old_table[i];
            HashTable_insert_internal(hashtable, entry.key, entry.value);
        }
    }

    // free the old table
    free(old_table);
}

/**
 * Insert an item into the HashTable using a given key
 * The value is appended to the Vector associated with given key
 * Parameters:
 *      hashtable - The HashTable to insert the key to
 *      key - The key to insert the item to
 *      value - The value to insert
 * Returns:
 *      0 on success, -1 on failure
 */
int HashTable_insert(HashTable_t *hashtable, hkey_t key, hvalue_t value) {
    HashTable_insert_internal(hashtable, key, value);

    // check if table should be grown
    double load_factor = (double) hashtable->num_keys / (double) hashtable->num_buckets;
    if (load_factor > GROWTH_THRESHHOLD) {
        HashTable_grow(hashtable);
    }

    return 0;
}

/**
 * Returns the value associated with a given key
 * Parameters:
 *      hashtable - The HashTable to search
 *      key - The key to search for
 * Returns:
 *      If key was found: value associated with that key
 *      Otherwise: value defined by not_in_table
 */
hvalue_t HashTable_get(HashTable_t *hashtable, hkey_t key) {
    size_t index = HashTable_lookup(hashtable, key);
    return hashtable->table[index].value;
    if (hashtable->table[index].key != bucket_empty) {
        return hashtable->table[index].value;
    }
    else {
        return NULL;
    }
}

/**
 * Check if the HashTable contains a given key
 * Parameters:
 *      hashtable - The HashTable to search
 *      key - The key to search for
 * Returns:
 *      1 if the key is found, 0 otherwise
 */
int HashTable_contains(HashTable_t *hashtable, hkey_t key) {
    size_t index = HashTable_lookup(hashtable, key);
    return hashtable->table[index].key != bucket_empty;
}