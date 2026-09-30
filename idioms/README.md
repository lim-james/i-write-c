# Idioms

One way I like learning a language, say C++, is to learn how to write idiomatic
code. It exposes you to the practices and general direction of the language.

## Raw thoughts

- `goto cleanup` is pretty neat. And it only works because C doesn't have
exceptions to throw. Its exceptional control path is just return errno
- `__typeof__` for pre C23 builts, `typeof` introduced in C23
- `__extension__`
- temp variables in macros are best marked with trailing `_` to prevent conflicts
- macro arguments are best used with () because you force them to be evaluated
as one independent expression e.g. `#define mul(x, y) x * y` but you call it as 
`add(2 + 5, 3)`

