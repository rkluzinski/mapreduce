#include <stdlib.h>     // for calloc and free
#include <string.h>
#include "hashtable.h"

// global constants
const static int INITIAL_CAPACITY = 8;
const static double GROWTH_THRESHHOLD = 0.75;
const ht_value_t key_not_in_table = NULL;

/**
 * A C style constructor for Node
 * Parameters:
 *      key - The key for this node
 * Returns:
 *      The pointer to the new Node object
 */
Node_t *Node_create(ht_key_t key, ht_value_t value) {
    Node_t *node = malloc(sizeof(Node_t));
    
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
void Node_destroy(Node_t *node) {
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
        hashtable->unique_keys = 0;
        hashtable->num_buckets = INITIAL_CAPACITY;
        
        // zero initialized to mark all buckets as empty
        // NULL values denote an empty bucket
        hashtable->buckets = calloc(sizeof(Node_t), hashtable->num_buckets);
        if (hashtable->buckets == NULL) {
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
    // destroy all nodes created by HashTable
    for (int i = 0; i < hashtable->num_buckets; i++) {
        if (hashtable->buckets[i] != NULL) {
            Node_destroy(hashtable->buckets[i]);
        }
    }

    free(hashtable->buckets);
    free(hashtable);
}

/**
 * Returns the hash of a given key
 * Uses DJB2 hashing algorithm provided with assignment spec
 * Parameters:
 *      key - The key to hash
 *      num_buckets - The number of buckets in the table
 */
size_t HashTable_hash(ht_key_t key, size_t num_buckets) {
    size_t hash = 5381;
    for (size_t c = *key; *key != 0; key++) {
        hash = hash * 33 + c;
    }

    // TODO change to bitmask
    return hash % num_buckets;
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
size_t HashTable_index_of_key(HashTable_t *hashtable, ht_key_t key) {
    size_t index = HashTable_hash(key, hashtable->num_buckets);

    // find key or first empty spot using linear probing
    Node_t **buckets = hashtable->buckets;
    while (buckets[index] != NULL && strcmp(buckets[index]->key, key) != 0) {
        index = ++index % hashtable->num_buckets;
    }

    /**
     * The HashTable is always grown before its filled
     * so its safe to assume there will always be an open spot
     */

    return index;
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
    Node_t **buckets = hashtable->buckets;

    // allocate new array
    hashtable->num_buckets *= 2;
    hashtable->buckets = calloc(sizeof(Node_t), hashtable->num_buckets);

    // copy nodes from olf 
    for (size_t i = 0; i < previous_size; i++) {
        if (buckets[i] != NULL) {
            Node_t *node = buckets[i];
            size_t index = HashTable_index_of_key(hashtable, node->key);
            hashtable->buckets[index] = node;
        }
    }

    // free the old arrya
    free(buckets);
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
int HashTable_insert(HashTable_t *hashtable, ht_key_t key, ht_value_t value) {
    Node_t **buckets = hashtable->buckets;
    size_t index = HashTable_index_of_key(hashtable, key);

    // if key did not exist, create new node
    if (buckets[index] == NULL) {
        ++hashtable->unique_keys;
        buckets[index] = Node_create(key, value);
        
        // check if bucket was created
        if (buckets[index] == NULL) {
            return -1;
        }
    }

    // check if table should be grown
    double load_factor = (double) hashtable->unique_keys / (double) hashtable->num_buckets;
    if (load_factor > GROWTH_THRESHHOLD) {
        HashTable_grow(hashtable);
    }

    return 0;
}

/**
 * Returns the value associated with a given key
 * Current implementation will just segfault
 * Parameters:
 *      hashtable - The HashTable to search
 *      key - The key to search for
 * Returns:
 *      If key was found: value associated with that key
 *      Otherwise: value defined by key_not_in_table
 */
ht_value_t HashTable_get(HashTable_t *hashtable, ht_key_t key) {
    size_t index = HashTable_index_of_key(hashtable, key);
    return hashtable->buckets[index]->value;
    // if (hashtable->buckets[index] != NULL) {
    //     return hashtable->buckets[index]->value;
    // }
    // else {
    //     return NULL;
    // }
}

/**
 * Check if the HashTable contains a given key
 * Parameters:
 *      hashtable - The HashTable to search
 *      key - The key to search for
 * Returns:
 *      1 if the key is found, 0 otherwise
 */
int HashTable_contains(HashTable_t *hashtable, ht_key_t key) {
    size_t index = HashTable_index_of_key(hashtable, key);
    return hashtable->buckets[index] != NULL;
}