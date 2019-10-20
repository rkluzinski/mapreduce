#ifndef VECTOR_H
#define VECTOR_H

// Define what the vector will store
typedef char *vector_data_t;

typedef struct {
    size_t size;
    size_t capacity;
    vector_data_t *data;
} Vector_t;

/**
 * A C style constructor for a Vector object
 * Returns:
 *      A pointer to the Vector
 */
Vector_t *Vector_create();

/**
 * A C Style destructor for a Vector object
 * Parameters:
 *      vector - The vector to be destroyed
 */
void Vector_destroy(Vector_t *vector);

/**
 * Get the szie of the vector
 * Parameters:
 *      vector - The vector to get the size of
 * Returns:
 *      The number of items stored in the vector
 */
int Vector_size(Vector_t *vector);

/**
 * Insert a new item at the back of a Vector
 * Parameters:
 *      vector - The vector to insert to
 *      item - The item to insert
 */
void Vector_insert(Vector_t *vector, vector_data_t item);

/**
 * Get the item at the given index
 * Parameters:
 *      vector - The vector to get the item from
 *      index - The index of the item
 * Returns:
 *      The value of the given item
 */
vector_data_t Vector_get(Vector_t *vector, int index);

#endif