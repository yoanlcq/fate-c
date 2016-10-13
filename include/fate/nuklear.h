#ifndef FE_NUKLEAR_H
#define FE_NUKLEAR_H

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_MEMSET memset
#define NK_MEMCOPY memcpy
#define NK_SQRT sqrtf
#define NK_SIN sinf
#define NK_COS cosf
#define NK_STRTOD strtod
/* #define NK_DTOA dtoa */
#define NK_VSNPRINTF vsnprintf
#define NK_ASSERT fe_dbg_assert
#include <fate/stdc.h>
#include <fate/dbg.h>
#include <nuklear/nuklear.h>

#endif/* FE_NUKLEAR_H*/
