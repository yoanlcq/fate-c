/* Try this:
 * gcc -std=c11 -Wall -Iinclude src/tests/test_vec.c 
 * With and without -DFE_MATH_WITHOUT_VECTOR_EXTENSIONS
 */

#include <fate/math/u32v2.h>
#include <fate/math/u32v3.h>
#include <fate/math/u32v4.h>
#include <fate/math/i32v2.h>
#include <fate/math/i32v3.h>
#include <fate/math/i32v4.h>
#include <fate/math/f32v2.h>
#include <fate/math/f32v3.h>
#include <fate/math/f32v4.h>
#include <fate/math/u64v2.h>
#include <fate/math/u64v3.h>
#include <fate/math/u64v4.h>
#include <fate/math/i64v2.h>
#include <fate/math/i64v3.h>
#include <fate/math/i64v4.h>
#include <fate/math/f64v2.h>
#include <fate/math/f64v3.h>
#include <fate/math/f64v4.h>
#include <fate/math/wv2.h>
#include <fate/math/wv2.h>
#include <fate/math/wv3.h>

#include <assert.h>

int main(void) {
    #ifndef FE_MATH_WITHOUT_VECTOR_EXTENSIONS
    uint32_t r=0xaaaaaaaa, g=0xbbbbbbbb, b=0xcccccccc, a=0xdddddddd;
    fe_u32v4 v = (fe_u32v4){.r=r,.g=g,.b=b,.a=a};
    assert(v.vx[0] == r);
    assert(v.vx[1] == g);
    assert(v.vx[2] == b);
    assert(v.vx[3] == a);
#else
#error "There's no point in running this test !"
    #endif
    return 0;
}
