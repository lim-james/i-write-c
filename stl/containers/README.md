# Containers

Probably the biggest trait of C is that it literally has no pre-built
containers as part of its standard. Sure you could download a package
but I don't want that. I genuinely feel handwriting anything yourself
is packed with rich learning e.g. I studied hash maps, but I retained
it by building my own.


## Raw thoughts

- `typdef struct` is best practice
- C does not default values | [StackOverflow
workaround](https://stackoverflow.com/questions/1472138/c-default-arguments)
- C does not have function overloading | [Vargs
Workaround](https://en.wikipedia.org/wiki/Stdarg.h#varargs.h)
- These 2 points aren't exactly the best start...
- `malloc` has not option for alignment - got me wondering how does it handle
alignment then? the answer seems to default to `alignas(max_align_t)`
- i need to include `stdbool.h` for `bool`
- i'm loving the build times
- `free(NULL)` is fine
