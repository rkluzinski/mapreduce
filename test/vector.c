#include <stdio.h>
#include <assert.h>

#include "../src/vector.h"

void test_growth() {
    const int num_items = 1024;
    vector_data_t mock_item = "Hello, World";

    Vector_t *vector = Vector_create();

    for (int i = 0; i < num_items; i++) {
        Vector_insert(vector, mock_item);
    }

    assert(Vector_size(vector) == num_items);

    Vector_destroy(vector);
}

void test_access() {
    const int num_items = 5;
    vector_data_t mock_item_list[] = {
        "1", "2", "3", "4", "5"
    };

    Vector_t *vector = Vector_create();

    for (int i = 0; i < num_items; i++) {
        Vector_insert(vector, mock_item_list[i]);
    }

    assert(Vector_size(vector) == num_items);

    for (int i = 0; i < num_items; i++) {
        vector_data_t item = Vector_get(vector, i);
        assert(item == mock_item_list[i]);
    }

    Vector_destroy(vector);
}

int main() {
    fputs("Testing Vector: ", stdout);
    
    test_growth();
    test_access();

    fputs("Passed\n", stdout);
    return 0;
}