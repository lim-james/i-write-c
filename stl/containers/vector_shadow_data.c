#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef struct VectorIntHeader {
    size_t size;
    size_t capacity;
} VectorIntHeader;

VectorIntHeader* _fetch_header(int* arr) {
    return (VectorIntHeader*)arr - 1;
}

size_t capacity(int* arr) { return _fetch_header(arr)->capacity; }
size_t size(int* arr) { return _fetch_header(arr)->size; }
bool is_empty(int* arr) { return _fetch_header(arr)->size == 0; }
bool _at_capacity(int* arr) { return size(arr) == capacity(arr); }

int* make_vector_int(void) {
    VectorIntHeader* header = malloc(sizeof(*header) + sizeof(int));
    header->size = 0;
    header->capacity = 1;
    return (int*)(header + 1);
}

void destroy_vector_int(int* arr) {
    free(_fetch_header(arr));
}

int _grow(int** arr) {
    VectorIntHeader* header = _fetch_header(*arr);

    static const size_t GROWTH_FACTOR = 2;
    const size_t new_capacity = header->capacity == 0 ? 1 : header->capacity * GROWTH_FACTOR;
    const size_t new_region_size = sizeof(*header) + sizeof(**arr) * new_capacity;
    VectorIntHeader* new_region = realloc(header, new_region_size);
    if (new_region == NULL) return -1;

    *arr = (int*)(new_region + 1);
    _fetch_header(*arr)->capacity = new_capacity;

    return 0;
}

void push_back(int** arr, int value) {
    if (_at_capacity(*arr)) _grow(arr);
    VectorIntHeader* header = _fetch_header(*arr);
    assert(header->size < header->capacity);
    (*arr)[header->size] = value;
    ++header->size;
}

int main(void) {
    int* arr = make_vector_int();

    assert(is_empty(arr));
    assert(size(arr) == 0);
    assert(capacity(arr) == 1);

    push_back(&arr, 0);
    push_back(&arr, 1);
    push_back(&arr, 2);

    assert(false == is_empty(arr));
    assert(size(arr) == 3);

    assert(arr[0] == 0);
    assert(arr[1] == 1);
    assert(arr[2] == 2);

    destroy_vector_int(arr);
    return 0;
}
