#include <stdio.h>
typedef struct foo foo;
struct foo {
    int fo;
};
void foo_print(foo f) {
    puts("foo");
}
void foo__set_fo(foo f, int fo) {
    f.fo = fo;
}
int foo__get_fo(foo f) {
    return f.fo;
}
#define foo__fo(x) (x.fo)
#define $(x,m) \
    _Generic(x, \
        foo: foo##_##m)(x)
#define $$(x,m,v) \
    _Generic(x, \
        foo: foo##_##m)(x,v)
int main(void) {
    foo ex;
    printf("size = %u\n", (unsigned)sizeof(ex));
    $(ex, print);
    $$(ex, _set_fo, 40);
    return $(ex, _get_fo);
}
