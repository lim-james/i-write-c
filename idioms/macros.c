#include <assert.h>

#define exchange(a, b) __extension__ ({            \
    __typeof__(a)* exchange_ptr_ = &(a);           \
    __typeof__(a)  exchange_old_ = *exchange_ptr_; \
    *exchange_ptr_ = (b);                          \
    exchange_old_;                                 \
})

int main(void) {
    int a = 5;
    int b = exchange(a, 2);
    assert(a == 2);
    assert(b == 5);
}
