#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int failing_fn(void) {
    bool is_successful = false;
    if (is_successful) return 0;
    return 1;
}

void foo() {
    printf("success!");
}

int* make_unique(int x) {
    int* ptr = malloc(sizeof(ptr));
    *ptr = x;
    return ptr;
}

int main(void) {
    int* int_ptr = make_unique(5);

    int errcode = failing_fn();
    if (errcode) goto cleanup_main;

    foo();

cleanup_main:
    free(int_ptr);
    return 0;
}
