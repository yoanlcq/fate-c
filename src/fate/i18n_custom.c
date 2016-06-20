#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <inttypes.h>

typedef struct {
    const char *fmt;
    const enum {
        FE_FMT_POINTER,
        FE_FMT_CHAR,
        FE_FMT_STRING,
        FE_FMT_FLOAT,
        FE_FMT_DOUBLE,
        FE_FMT_SSIZE_T,
        FE_FMT_SIZE_T,
        FE_FMT_INT8_T,
        FE_FMT_INT16_T,
        FE_FMT_INT32_T,
        FE_FMT_INT64_T,
        FE_FMT_UINT8_T,
        FE_FMT_UINT16_T,
        FE_FMT_UINT32_T,
        FE_FMT_UINT64_T
    } val_type;
    const union {
        const void*  as_pointer;
        char         as_char;
        const char * as_string;
        float        as_float;
        double       as_double;
        ssize_t      as_ssize_t;
        size_t       as_size_t;
        int8_t       as_int8_t;
        int16_t      as_int16_t;
        int32_t      as_int32_t;
        int64_t      as_int64_t;
        uint8_t      as_uint8_t;
        uint16_t     as_uint16_t;
        uint32_t     as_uint32_t;
        uint64_t     as_uint64_t;
    } val;
    long width;
    unsigned long precision;
} fe_i18n_param;

size_t fe_i18n_print_real(const char *fmt, const fe_i18n_param *params, size_t param_count) {
    const char *cur=fmt;
    for(;;) {
        const char *dlr = strchr(cur, '$');
        if(!dlr) {
            fwrite(cur, 1, strlen(cur), stdout);
            break;
        }
        if(dlr[1]=='$') {
            fwrite(cur, 1, dlr+1-cur, stdout);
            cur = dlr+2;
            continue;
        }
        char *end;
        size_t idx = strtoul(dlr+1, &end, 10);
        assert(idx<param_count && "Parameter index is out of bounds.");
        const fe_i18n_param *p = params+idx;
        char fmtbuf[256];
        if(p->precision)
            snprintf(fmtbuf, sizeof(fmtbuf), "%%%ld.%lu%s", p->width ? p->width : 1, p->precision, p->fmt);
        else
            snprintf(fmtbuf, sizeof(fmtbuf), "%%%ld%s", p->width ? p->width : 1, p->fmt);
        fwrite(cur, 1, dlr-cur, stdout);
        switch(params[idx].val_type) {
        case FE_FMT_POINTER : printf(fmtbuf, p->val.as_pointer);  break;
        case FE_FMT_CHAR    : printf(fmtbuf, p->val.as_char);     break;
        case FE_FMT_STRING  : printf(fmtbuf, p->val.as_string);   break;
        case FE_FMT_FLOAT   : printf(fmtbuf, p->val.as_float);    break;
        case FE_FMT_DOUBLE  : printf(fmtbuf, p->val.as_double);   break;
        case FE_FMT_SSIZE_T : printf(fmtbuf, p->val.as_ssize_t);  break;
        case FE_FMT_SIZE_T  : printf(fmtbuf, p->val.as_size_t);   break;
        case FE_FMT_INT8_T  : printf(fmtbuf, p->val.as_int8_t);   break;
        case FE_FMT_UINT8_T : printf(fmtbuf, p->val.as_uint8_t);  break;
        case FE_FMT_INT16_T : printf(fmtbuf, p->val.as_int16_t);  break;
        case FE_FMT_UINT16_T: printf(fmtbuf, p->val.as_uint16_t); break;
        case FE_FMT_INT32_T : printf(fmtbuf, p->val.as_int32_t);  break;
        case FE_FMT_UINT32_T: printf(fmtbuf, p->val.as_uint32_t); break;
        case FE_FMT_INT64_T : printf(fmtbuf, p->val.as_int64_t);  break;
        case FE_FMT_UINT64_T: printf(fmtbuf, p->val.as_uint64_t); break;
        }
        cur = end;
    }
}
#define COUNTOF(x) 3
#define fe_fmt_pointer(var,width,precision)  { "p",        FE_FMT_POINTER,  .val.as_pointer  =(const void*)var, width, precision }
#define fe_fmt_char(var,width,precision)     { "c",        FE_FMT_CHAR,     .val.as_char     =(       char)var, width, precision }
#define fe_fmt_string(var,width,precision)   { "s",        FE_FMT_STRING,   .val.as_string   =(const char*)var, width, precision }
#define fe_fmt_float(var,width,precision)    { "f",        FE_FMT_FLOAT,    .val.as_float    =(      float)var, width, precision }
#define fe_fmt_float_e(var,width,precision)  { "e",        FE_FMT_FLOAT,    .val.as_float    =(      float)var, width, precision }
#define fe_fmt_float_E(var,width,precision)  { "E",        FE_FMT_FLOAT,    .val.as_float    =(      float)var, width, precision }
#define fe_fmt_float_g(var,width,precision)  { "g",        FE_FMT_FLOAT,    .val.as_float    =(      float)var, width, precision }
#define fe_fmt_float_G(var,width,precision)  { "G",        FE_FMT_FLOAT,    .val.as_float    =(      float)var, width, precision }
#define fe_fmt_double(var,width,precision)   { "lf",       FE_FMT_DOUBLE,   .val.as_double   =(     double)var, width, precision }
#define fe_fmt_double_e(var,width,precision) { "le",       FE_FMT_DOUBLE,   .val.as_double   =(     double)var, width, precision }
#define fe_fmt_double_E(var,width,precision) { "lE",       FE_FMT_DOUBLE,   .val.as_double   =(     double)var, width, precision }
#define fe_fmt_double_g(var,width,precision) { "lg",       FE_FMT_DOUBLE,   .val.as_double   =(     double)var, width, precision }
#define fe_fmt_double_G(var,width,precision) { "lG",       FE_FMT_DOUBLE,   .val.as_double   =(     double)var, width, precision }
#define fe_fmt_ssize_t(var,width,precision)  { PRIssize_t, FE_FMT_SSIZE_T,  .val.as_ssize_t  =(    ssize_t)var, width, precision }
#define fe_fmt_size_t(var,width,precision)   { PRIsize_t,  FE_FMT_SIZE_T,   .val.as_size_t   =(     size_t)var, width, precision }
#define fe_fmt_osize_t(var,width,precision)  { PRIosize_t, FE_FMT_SIZE_T,   .val.as_size_t   =(     size_t)var, width, precision }
#define fe_fmt_xsize_t(var,width,precision)  { PRIxsize_t, FE_FMT_SIZE_T,   .val.as_size_t   =(     size_t)var, width, precision }
#define fe_fmt_Xsize_t(var,width,precision)  { PRIXsize_t, FE_FMT_SIZE_T,   .val.as_size_t   =(     size_t)var, width, precision }
#define fe_fmt_s8(var,width,precision)       { PRId8,      FE_FMT_INT8_T,   .val.as_int8_t   =(     int8_t)var, width, precision }
#define fe_fmt_i8(var,width,precision)       { PRIi8,      FE_FMT_INT8_T,   .val.as_int8_t   =(     int8_t)var, width, precision }
#define fe_fmt_u8(var,width,precision)       { PRIu8,      FE_FMT_UINT8_T,  .val.as_uint8_t  =(    uint8_t)var, width, precision }
#define fe_fmt_o8(var,width,precision)       { PRIo8,      FE_FMT_UINT8_T,  .val.as_uint8_t  =(    uint8_t)var, width, precision }
#define fe_fmt_x8(var,width,precision)       { PRIx8,      FE_FMT_UINT8_T,  .val.as_uint8_t  =(    uint8_t)var, width, precision }
#define fe_fmt_X8(var,width,precision)       { PRIX8,      FE_FMT_UINT8_T,  .val.as_uint8_t  =(    uint8_t)var, width, precision }
#define fe_fmt_s16(var,width,precision)      { PRId16,     FE_FMT_INT16_T,  .val.as_int16_t  =(    int16_t)var, width, precision }
#define fe_fmt_i16(var,width,precision)      { PRIi16,     FE_FMT_INT16_T,  .val.as_int16_t  =(    int16_t)var, width, precision }
#define fe_fmt_u16(var,width,precision)      { PRIu16,     FE_FMT_UINT16_T, .val.as_uint16_t =(   uint16_t)var, width, precision }
#define fe_fmt_o16(var,width,precision)      { PRIo16,     FE_FMT_UINT16_T, .val.as_uint16_t =(   uint16_t)var, width, precision }
#define fe_fmt_x16(var,width,precision)      { PRIx16,     FE_FMT_UINT16_T, .val.as_uint16_t =(   uint16_t)var, width, precision }
#define fe_fmt_X16(var,width,precision)      { PRIX16,     FE_FMT_UINT16_T, .val.as_uint16_t =(   uint16_t)var, width, precision }
#define fe_fmt_s32(var,width,precision)      { PRId32,     FE_FMT_INT32_T,  .val.as_int32_t  =(    int32_t)var, width, precision }
#define fe_fmt_i32(var,width,precision)      { PRIi32,     FE_FMT_INT32_T,  .val.as_int32_t  =(    int32_t)var, width, precision }
#define fe_fmt_u32(var,width,precision)      { PRIu32,     FE_FMT_UINT32_T, .val.as_uint32_t =(   uint32_t)var, width, precision }
#define fe_fmt_o32(var,width,precision)      { PRIo32,     FE_FMT_UINT32_T, .val.as_uint32_t =(   uint32_t)var, width, precision }
#define fe_fmt_x32(var,width,precision)      { PRIx32,     FE_FMT_UINT32_T, .val.as_uint32_t =(   uint32_t)var, width, precision }
#define fe_fmt_X32(var,width,precision)      { PRIX32,     FE_FMT_UINT32_T, .val.as_uint32_t =(   uint32_t)var, width, precision }
#define fe_fmt_s64(var,width,precision)      { PRId64,     FE_FMT_INT64_T,  .val.as_int64_t  =(    int64_t)var, width, precision }
#define fe_fmt_i64(var,width,precision)      { PRIi64,     FE_FMT_INT64_T,  .val.as_int64_t  =(    int64_t)var, width, precision }
#define fe_fmt_u64(var,width,precision)      { PRIu64,     FE_FMT_UINT64_T, .val.as_uint64_t =(   uint64_t)var, width, precision }
#define fe_fmt_o64(var,width,precision)      { PRIo64,     FE_FMT_UINT64_T, .val.as_uint64_t =(   uint64_t)var, width, precision }
#define fe_fmt_x64(var,width,precision)      { PRIx64,     FE_FMT_UINT64_T, .val.as_uint64_t =(   uint64_t)var, width, precision }
#define fe_fmt_X64(var,width,precision)      { PRIX64,     FE_FMT_UINT64_T, .val.as_uint64_t =(   uint64_t)var, width, precision }
#define fe_i18n_print(fmt, ...) do { \
    const fe_i18n_param fe_i18n_the_params[] = {__VA_ARGS__}; \
    fe_i18n_print_real(fmt, fe_i18n_the_params, COUNTOF(fe_i18n_the_params)); \
} while(0)

/*
int main(void) {
    const char *objname = "Mystical potion";
    const float objprice = 42.f;

    fe_i18n_print(
        // Translators: $0 points to the object name, and $1 is its price.
        "The '$0' object costs $1$$ and there are $2 of them.\n", 
        fe_fmt_string(objname, -20, 0),
        fe_fmt_float(objprice, 0, 2),
        fe_fmt_u32(32, 0, 0)
    );
    return 0;
}
*/
