#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

typedef struct VectorI {
    int*   _data;
    size_t _size;
    size_t _capacity;
} VectorI;

size_t size(const VectorI* this) { return this->_size; }
size_t capacity(const VectorI* this) { return this->_capacity; }
bool empty(const VectorI* this) { return this->_size == 0; }
bool _is_at_capacity(const VectorI* this) { return this->_size == this->_capacity; }

VectorI make(void) {
    VectorI vec;
    vec._data     = NULL; 
    vec._size     = 0;
    vec._capacity = 0;
    return vec;
}

void destroy(const VectorI* this) {
    free(this->_data); 
}

void reserve(VectorI* this, size_t capacity) {
    if (capacity <= this->_capacity) return;

    int* new_memory_region = malloc(capacity * sizeof(int));
    int* old_memory_region = this->_data;

    for (size_t i = 0; i < this->_size; ++i) {
        new_memory_region[i] = old_memory_region[i];
    }
    this->_data     = new_memory_region;
    this->_capacity = capacity;
    free(old_memory_region);
}

void resize(VectorI* this, size_t size, int value) {
    if (size > this->_capacity) reserve(this, size);

    for (size_t i = this->_size; i < size; ++i) {
        this->_data[i] = value;
    }

    this->_size = size;
}

int get(const VectorI* this, size_t index) {
    assert(index < this->_size);
    return this->_data[index];
}

void push_back(VectorI* this, int value) {
    if (_is_at_capacity(this)) {
        reserve(this, this->_size == 0 ? 1 : this->_size << 1);
    }

    this->_data[this->_size] = value;
    ++this->_size;
}

void pop_back(VectorI* this) {
    assert(!empty(this));
    --this->_size;
}

int main(void) {
    VectorI vec = make();
    const VectorI vec2 = make();
    destroy(&vec2);
    destroy(&vec);
/*
    {
        VectorI veci = make();
        assert(empty(&veci));
        push_back(&veci, 4);
        push_back(&veci, 5);
        push_back(&veci, 6);

        assert(false == empty(&veci));
        assert(size(&veci) == 3);
        assert(get(&veci, 0) == 4);
        assert(get(&veci, 1) == 5);
        assert(get(&veci, 2) == 6);

        pop_back(&veci);
        pop_back(&veci);
        pop_back(&veci);
        assert(empty(&veci));

        destroy(&veci);
    }

    {
        VectorI veci = make();
        push_back(&veci, 0);
        push_back(&veci, 1);
        push_back(&veci, 2);
        push_back(&veci, 3);

        assert(size(&veci) == 4);
        resize(&veci, 4, 0);
        assert(size(&veci) == 4);
        resize(&veci, 3, 0);
        assert(size(&veci) == 3);

        destroy(&veci);
    }

    {
        VectorI veci = make();
        reserve(&veci, 2);

        assert(capacity(&veci) == 2);
        assert(size(&veci) == 0);

        resize(&veci, 2, 3);
        assert(size(&veci) == 2);
        assert(get(&veci, 0) == 3);
        assert(get(&veci, 1) == 3);

        destroy(&veci);
    }

*/

    return 0;
}
