
/*
 * Pros :
 * - Maintainable, and stable. The C compiler is better at type inference than me 
 *   (and at eliminating noisy qualifiers);
 * - Supports macros! Merry Christmas!
 * - No need to declare dummy functions.
 * - Fairly usable.
 * Cons :
 * - Huge error messages. One wrong line can flood the screen.
 * - Not supporting MSVC anymore. Might really be an issue the day I target the 
 *   Xbox (and probably other consoles that use older versions of GCC).
 *   Look at this mess : '$' identifer, statement expressions, typeof, built-ins, 
 *   '## __VA_ARGS__' extension...
 *   Alternatively, I could compile into assembly using the latest GCC (using e.g Intel syntax),
 *   then compile the assembly with the compiler I wish.
 * - Having to have a script that regenerates the whole macro every time;
 * - The '_p' suffix for functions that take a pointer as a first arg.
 *   (though this could be avoided by using macros ?)
 */

#ifndef __GNUC__
#error "This ain't gonna compile on non GCC-compilers."
#endif

#define $(x,f,...) \
    ({ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wimplicit-function-declaration\"") \
    __builtin_choose_expr( \
        __builtin_types_compatible_p(__typeof__(x), cb_cube), \
        cb_cube_##f(((union {cb_cube to; __typeof__(x) from;}){.from=(x)}).to, ##__VA_ARGS__), \
    __builtin_choose_expr( \
        __builtin_types_compatible_p(__typeof__(x), cb_cube*), \
        cb_cube_p_##f(((union {cb_cube* to; __typeof__(x) from;}){.from=(x)}).to, ##__VA_ARGS__), \
    __builtin_choose_expr( \
        __builtin_types_compatible_p(__typeof__(x), cb_tri), \
        cb_tri_##f(((union {cb_tri to; __typeof__(x) from;}){.from=(x)}).to, ##__VA_ARGS__), \
    __builtin_choose_expr( \
        __builtin_types_compatible_p(__typeof__(x), cb_rgba32), \
        cb_rgba32_##f(((union {cb_rgba32 to; __typeof__(x) from;}){.from=(x)}).to, ##__VA_ARGS__), \
    __builtin_choose_expr( \
        __builtin_types_compatible_p(__typeof__(x), cb_vertex), \
        cb_vertex_##f(((union {cb_vertex to; __typeof__(x) from;}){.from=(x)}).to, ##__VA_ARGS__), \
    (void)0))))); \
    _Pragma("GCC diagnostic pop") \
     })

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

typedef struct { uint8_t rgba[4]; } cb_rgba32;
typedef struct { float x,y,z; cb_rgba32 color; } cb_vertex;
typedef struct { int foo; cb_vertex vertices[4]; } cb_cube;
typedef struct { int foo; cb_vertex vertices[3]; } cb_tri;

//hello_no_arg
static inline void cb_cube_hello_no_arg(cb_cube c) {
    puts("I'm a function, supposed to be overridden by a macro !");
}
#define cb_cube_hello_no_arg(c) \
    puts("I'm a macro, overriding a function !")
//hello
static inline void cb_cube_hello(cb_cube c, int foo) {
    puts(__func__);
}
static inline void cb_cube_p_allocv(cb_cube *c, size_t cnt) {}
//hello
static inline void cb_tri_hello(cb_tri t, int foo) {
    puts(__func__);
}
static inline cb_vertex* cb_tri_vertices(cb_tri t) { return NULL; }
#define                  cb_tri_vertices(t) ((t).vertices)
static inline size_t cb_tri_vertex_count(cb_tri t) {return 3;}
static inline cb_rgba32 cb_vertex_rgba32(cb_vertex v) {
    return (cb_rgba32){{13, 128, 42, 255}};
}
static inline uint8_t cb_rgba32_r(cb_rgba32 c) { return c.rgba[0]; }

int main(void) {
    cb_cube c;
    cb_tri t;
    $(&c, allocv, 1);
    $(c, hello_no_arg);
    $(c, hello, 42);
    $(t, hello, 42);
    cb_rgba32 rgba = $($(t, vertices)[0], rgba32);
    printf("rgba (host endianness) = %x\n", *(int*)&rgba);
//  uint8_t r = t..vertices[0]..rgba32..r
    uint8_t r = $($($(t,vertices)[0], rgba32), r);
//  uint8_t r = cb_rgba32_r(cb_vertex_rgba32(cb_tri_vertices(t)[0]))
//  Or :
//  rgba32 rgba = cb_vertex_rgba32(cb_tri_vertices(t)[0])
//  uint8_t r = cb_rgba32_r(rgba);
    printf("r = %"PRIu8"\n", r);
    return 0;
}


/*
typedef union {
    funcptr0 p0;
    funcptr1 p1;
} all_funcptrs;

int main(void) {
    cb_cube_(c); => _Pragma(push macro c____type cb_cube)
    _(_(c,hello_no_arg), foo); => c____type##hello_no_arg(c);
}
*/
