#include "stdlib.h"     //for malloc, realloc and free
#include "vector.h"

const static int INITIAL_CAPACITY = 8;

/**
 * A C style constructor for a Vector object
 * Returns:
 *      A pointer to the Vector
 */
Vector_t *Vector_create() {
    Vector_t *vector = (Vector_t *) malloc(sizeof(Vector_t));

    if (vector != NULL) {
        vector->size = 0;
        vector->capacity = INITIAL_CAPACITY;
        vector->data = malloc(sizeof(vec_data_t) * vector->capacity);
    }

    return vector;
}

/**
 * A C Style destructor for a Vector object
 * Parameters:
 *      vector - The vector to be destroyed
 */
void Vector_destroy(Vector_t *vector) {
    free(vector->data);
    free(vector);
}

/**
 * Get the szie of the vector
 * Parameters:
 *      vector - The vector to get the size of
 * Returns:
 *      The number of items stored in the vector
 */
int Vector_size(Vector_t *vector) {
    return vector->size;
}

/**
 * Insert a new item at the back of a Vector
 * Parameters:
 *      vector - The vector to insert to
 *      item - The item to insert
 */
void Vector_insert(Vector_t *vector, vec_data_t item) {
    vector->data[vector->size++] = item;

    // grow vector if at capacity
    if (vector->size == vector->capacity) {
        vector->capacity *= 2;
        vector->data = realloc(vector->data,
            sizeof(Vector_t) * vector->capacity);
    }
}

/**
 * Get the item at the given index
 * Parameters:
 *      vector - The vector to get the item from
 *      index - The index of the item
 * Returns:
 *      The value of the given item
 */
vec_data_t Vector_get(Vector_t *vector, int index) {
    return vector->data[index];
}