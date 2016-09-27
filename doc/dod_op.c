
#ifndef __GNUC__
#error "This ain't gonna compile on non GCC-compilers."
#endif

#define $typesel(x,t,f,...) \
    __builtin_choose_expr( \
        __builtin_types_compatible_p(__typeof__(x), t), \
        t##_##f(*(t*)&(x), ##__VA_ARGS__), (void)0 \
    )
#define $(x,f,...) \
    $typesel(x,cb_cube,f,##__VA_ARGS__); \
    $typesel(x,cb_tri,f,##__VA_ARGS__); 


#include <stdio.h>

typedef struct { int foo; } cb_cube;
typedef struct { short foo; long long bar; } cb_tri;

static inline void cb_cube_hello0(cb_cube c) {
    printf("cube void\n");
}
static inline void cb_cube_hello(cb_cube c, int foo) {
    printf("cube\n");
}
static inline void cb_tri_hello(cb_tri t, int foo) {
    printf("tri\n");
}

int main(void) {
    cb_cube c;
    cb_tri t;
    $(c, hello0);
    $(c, hello, 42);
    $(t, hello, 42);
    return 0;
}
