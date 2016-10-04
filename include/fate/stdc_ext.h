#ifndef FE_STDC_EXT_H
#define FE_STDC_EXT_H

#include <fate/defs.h>
#include <assert.h>

/*! \brief Macro for checking if we're using the C11 standard. */
#if __STDC_VERSION__ >= 201112L
#define FE_C11_SUPPORT
#endif

/*! \brief Define <tt>__func__</tt> to something else, only if the used
 *         C standard does not support it. */
#if __STDC_VERSION__ < 199901L
#ifndef __GNUC__
#define __func__ "<somefunction>"
#endif
#endif

#ifdef _MSC_VER
#define restrict __restrict
#endif

/*! \brief Definition for M_PI, since it may be missing with std=c11. */
#define FE_PI_F 3.14159265358979323846264338327f
#define FE_PI   3.14159265358979323846264338327


/*! \brief Forces a compilation error if condition is true. Evaluates to 0. */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))

#if __DOXYGEN__ || defined(__GNUC__)
/*! \brief Ensures the given expression is an array.
 *
 * Always evaluates to 0, but causes a compilation error if the given 
 * expression is not an array.
 *
 * This macro expands to 0 if the compiler is not GCC.
 * \return 0
 */
#define MUST_BE_ARRAY(a) \
     BUILD_BUG_ON_ZERO(__builtin_types_compatible_p(typeof(a), typeof(&a[0])))
#else
#define MUST_BE_ARRAY(a) 0
#endif


/*! \brief Returns the number of elements of an array which size can be 
 *         computed at compile-time. 
 * 
 * Please don't use it on pointers. It ain't wizardry.
 */
#define fe_countof(arr) (sizeof(arr)/sizeof((arr)[0]) + MUST_BE_ARRAY(arr))

#ifdef FE_C11_SUPPORT
    #ifdef _MSC_VER
        #define fe_static_assertion(pred,msg) static_assert(pred, msg)
    #elif defined(__GNUC__)
        #define fe_static_assertion(pred,msg) _Static_assert(pred, msg)
    #endif
#else
#error "Could not define fe_static_assertion for now."
#endif


typedef unsigned long ulong;
#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#include <sys/types.h>
#endif

#ifdef FE_TARGET_WINDOWS
#define PRIssize_t "Id"
#define PRIsize_t  "Iu"
#define PRIosize_t "Io"
#define PRIxsize_t "Ix"
#define PRIXsize_t "IX"
#else
#define PRIssize_t "zd"
#define PRIsize_t  "zu"
#define PRIosize_t "zo"
#define PRIxsize_t "zx"
#define PRIXsize_t "zX"
#endif

#include <stdint.h>

typedef  int8_t  fe_s8;
typedef uint8_t  fe_u8;
typedef  int16_t fe_s16;
typedef uint16_t fe_u16;
typedef  int32_t fe_s32;
typedef uint32_t fe_u32;
typedef  int64_t fe_s64;
typedef uint64_t fe_u64;
typedef    float fe_f32;
typedef   double fe_f64;
typedef long double  fe_lf64;
#ifdef __SIZEOF_INT128__
#define FE_I128_SUPPORTED
typedef   signed __int128 fe_s128;
typedef unsigned __int128 fe_u128;
#endif

#ifdef __SIZEOF_FLOAT128__
#define FE_F128_SUPPORTED
typedef __float128 fe_f128;
#endif

#endif /* FE_STDC_EXT_H */
