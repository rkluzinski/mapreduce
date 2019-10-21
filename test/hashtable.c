#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/hashtable.h"

// from the wikipedia page on hash tables
const char *g_message = "A basic requirement is that the function should \
provide a uniform distribution of hash values. A non-uniform distribution \
increases the number of collisions and the cost of resolving them. Uniformity \
is sometimes difficult to ensure by design, but may be evaluated empirically \
using statistical tests, e.g., a Pearson's chi-squared test for discrete \
uniform distributions.[6][7] The distribution needs to be uniform only for \
table sizes that occur in the application. In particular, if one uses dynamic \
resizing with exact doubling and halving of the table size, then the hash \
function needs to be uniform only when the size is a power of two. Here the \
index can be computed as some range of bits of the hash function. On the \
other hand, some hashing algorithms prefer to have the size be a prime \
number.[8] The modulus operation may provide some additional mixing; \
this is especially useful with a poor hash function. For open addressing \
schemes, the hash function should also avoid clustering, the mapping of two \
or more keys to consecutive slots. Such clustering may cause the lookup cost \
to skyrocket, even if the load factor is low and collisions are infrequent. \
The popular multiplicative hash[3] is claimed to have particularly poor \
clustering behavior.[8] Cryptographic hash functions are believed to provide \
good hash functions for any table size, either by modulo reduction or by bit \
masking[citation needed]. They may also be appropriate if there is a risk of \
malicious users trying to sabotage a network service by submitting requests \
designed to generate a large number of collisions in the server's hash tables. \
However, the risk of sabotage can also be avoided by cheaper methods (such as \
applying a secret salt to the data, or using a universal hash function). A \
drawback of cryptographic hashing functions is that they are often slower \
to compute, which means that in cases where the uniformity for any size is \
not necessary, a non-cryptographic hashing function might be preferable.";

void test_normal_use() {
    HashTable_t *hashtable = HashTable_create();

    HashTable_insert(hashtable, "apple", "1");
    HashTable_insert(hashtable, "boat", "1");
    HashTable_insert(hashtable, "cat", "1");

    assert(HashTable_get(hashtable, "apple") != NULL);
    assert(HashTable_get(hashtable, "boat") != NULL);
    assert(HashTable_get(hashtable, "cat") != NULL);

    assert(HashTable_get(hashtable, "banana") == NULL);
    assert(HashTable_get(hashtable, "car") == NULL);
    assert(HashTable_get(hashtable, "canada") == NULL);

    HashTable_destroy(hashtable);
}

void test_growth() {
    HashTable_t *hashtable = HashTable_create();

    char *message = strdup(g_message);
    char *token, *dummy = message;
    while ((token = strsep(&dummy, " \t\n\r")) != NULL) {
        HashTable_insert(hashtable, token, "1");
    }

    assert(HashTable_get(hashtable, "basic") != NULL);
    assert(HashTable_get(hashtable, "requirement") != NULL);
    assert(HashTable_get(hashtable, "algorithms") != NULL);
    assert(HashTable_get(hashtable, "modulo") != NULL);
    assert(HashTable_get(hashtable, "collisions") != NULL);
    assert(HashTable_get(hashtable, "malicious") != NULL);
    assert(HashTable_get(hashtable, "cryptographic") != NULL);

    free(message);
    HashTable_destroy(hashtable);
}

int main(int argc, char **argv) {
    fputs("Testing HashTable: ", stdout);   
    
    test_normal_use();
    test_growth();

    fputs("Passed\n", stdout);
    return 0;
}