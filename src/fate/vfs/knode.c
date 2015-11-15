#include <stdlib.h>
#include <fst/knode.h>

inline fst_knode_s8    *fst_knode_s8_alloc (void){return malloc(sizeof(fst_knode_s8 ));}
inline fst_knode_s16   *fst_knode_s16_alloc(void){return malloc(sizeof(fst_knode_s16));}
inline fst_knode_s32   *fst_knode_s32_alloc(void){return malloc(sizeof(fst_knode_s32));}
inline fst_knode_s64   *fst_knode_s64_alloc(void){return malloc(sizeof(fst_knode_s64));}
inline fst_knode_u8    *fst_knode_u8_alloc (void){return malloc(sizeof(fst_knode_u8 ));}
inline fst_knode_u16   *fst_knode_u16_alloc(void){return malloc(sizeof(fst_knode_u16));}
inline fst_knode_u32   *fst_knode_u32_alloc(void){return malloc(sizeof(fst_knode_u32));}
inline fst_knode_u64   *fst_knode_u64_alloc(void){return malloc(sizeof(fst_knode_u64));}
inline fst_knode_f32   *fst_knode_f32_alloc(void){return malloc(sizeof(fst_knode_f32));}
inline fst_knode_f64   *fst_knode_f64_alloc(void){return malloc(sizeof(fst_knode_f64));}
inline fst_knode_str8  *fst_knode_str8_alloc(void) {
    fst_knode_str8 *res = malloc(sizeof(fst_knode_str8));
    res->value = calloc(1, 1);
}
inline fst_knode_str32 *fst_knode_str32_alloc(void) {
    fst_knode_str32 *res = malloc(sizeof(fst_knode_str32));
    res->value = calloc(1, 4);
}
inline void fst_knode_dealloc_extras(fst_node *n) {
    if(n->value_type==FST_KNODE_STR8)
        free(((fst_knode_str8*)n)->value);
    else if(n->value_type==FST_KNODE_STR32)
        free(((fst_knode_str32*)n)->value);
}

/* TODO : Accessing nodes doesn't create them. There are no Void Nodes.
 * You don't change the type of nodes. That implies realloc(), which breaks hard links.
 * There's only Copy and Delete. Creating= Copying from primitives.
 * Copy from a Knode on a Knode can have the effect to convert the value to the correct type.
 * */

fst_hardlink *fst_knode_transfer_value(fst_hardlink *src, fst_hardlink *dst, int decimal_part_numdigits) {

    fst_hardlink *res;
    uint_fast32_t i, str32len;
    uint8_t *utf8;
    
    switch(n->value_type) {
    case FST_KNODE_NONE : 
        fprintf(stderr, "Error : Conversion to %s is forbidden on \"%s\", which is a %s.\n",
            fst_value_type_names[t][0], n->name, fst_node_type_names[n->type][0]);
        fst_abort();
        break;


    case FST_KNODE_STR8 : 
        switch(t) {
        case FST_KNODE_STR8: 
            res = fst_knode_str8_alloc(); 
            ((fst_knode_str8*)res)->value = realloc(((fst_knode_str8*)res)->value, 1 + strlen(((fst_knode_str8*)n)->value);
            strcpy(((fst_knode_str8*)res)->value, ((fst_knode_str8*)n)->value);
            break;
        case FST_KNODE_STR32: 
            res = fst_knode_str32_alloc(); 
            ((fst_knode_str32*)res)->value = realloc(((fst_knode_str32*)res)->value, 4*(1+strlen(((fst_knode_str8*)n)->value)));
            for(i=0 ; ((fst_knode_str8*)n)->value[i] ; ++i)
                ((fst_knode_str32*)res)->value[i] = ((fst_knode_str8*)n)->value[i];
            ((fst_knode_str32*)res)->value[i] = 0U;
            break; 
        case FST_KNODE_S8   : res = fst_knode_s8_alloc();  ((fst_knode_s8*) res)->value = strtol  (((fst_knode_str8*)n)->value, NULL, 0); break;
        case FST_KNODE_S16  : res = fst_knode_s16_alloc(); ((fst_knode_s16*)res)->value = strtol  (((fst_knode_str8*)n)->value, NULL, 0); break;
        case FST_KNODE_S32  : res = fst_knode_s32_alloc(); ((fst_knode_s32*)res)->value = strtol  (((fst_knode_str8*)n)->value, NULL, 0); break;
        case FST_KNODE_S64  : res = fst_knode_s64_alloc(); ((fst_knode_s64*)res)->value = strtoll (((fst_knode_str8*)n)->value, NULL, 0); break;
        case FST_KNODE_U8   : res = fst_knode_u8_alloc();  ((fst_knode_u8_*)res)->value = strtoul (((fst_knode_str8*)n)->value, NULL, 0); break;
        case FST_KNODE_U16  : res = fst_knode_u16_alloc(); ((fst_knode_u16*)res)->value = strtoul (((fst_knode_str8*)n)->value, NULL, 0); break;
        case FST_KNODE_U32  : res = fst_knode_u32_alloc(); ((fst_knode_u32*)res)->value = strtoul (((fst_knode_str8*)n)->value, NULL, 0); break;
        case FST_KNODE_U64  : res = fst_knode_u64_alloc(); ((fst_knode_u64*)res)->value = strtoull(((fst_knode_str8*)n)->value, NULL, 0); break;
        case FST_KNODE_F32  : res = fst_knode_f32_alloc(); ((fst_knode_f32*)res)->value = strtof  (((fst_knode_str8*)n)->value, NULL);    break;
        case FST_KNODE_F64  : res = fst_knode_f64_alloc(); ((fst_knode_f64*)res)->value = strtod  (((fst_knode_str8*)n)->value, NULL);    break;
        }
        break;


    case FST_KNODE_STR32:
        for(str32len=0 ; ((fst_knode_str32*)n)->value[str32len] ; ++str32len)
            ;
        /* Make an utf-8 string if needed (freed after the switch() below) */
        if(t != FST_KNODE_STR32) {
            utf8 = malloc(str32len+1);
            for(i=0 ; i<str32len ; ++i)
                utf8[i] = ((fst_knode_str32*)n)->value[i];
            utf8[i] = '\0';
        }
        switch(t) {
        case FST_KNODE_STR32: 
            res = fst_knode_str32_alloc();
            ((fst_knode_str32*)res)->value = realloc(((fst_knode_str32*)res)->value, 4*(str32len+1));
            memcpy(((fst_knode_str32*)res)->value, ((fst_knode_str32*)n)->value, 4*(str32len+1));
            break; 
        case FST_KNODE_STR8: 
            res = fst_knode_str8_alloc(); 
            ((fst_knode_str8*)res)->value = realloc(((fst_knode_str8*)res)->value, str32len+1);
            strcpy(((fst_knode_str8*)res)->value, utf8);
            break; 
        case FST_KNODE_S8   : res = fst_knode_s8_alloc();  ((fst_knode_s8 *)res)->value = strtol  (utf8, NULL, 0); break;
        case FST_KNODE_S16  : res = fst_knode_s16_alloc(); ((fst_knode_s16*)res)->value = strtol  (utf8, NULL, 0); break;
        case FST_KNODE_S32  : res = fst_knode_s32_alloc(); ((fst_knode_s32*)res)->value = strtol  (utf8, NULL, 0); break;
        case FST_KNODE_S64  : res = fst_knode_s64_alloc(); ((fst_knode_s64*)res)->value = strtoll (utf8, NULL, 0); break;
        case FST_KNODE_U8   : res = fst_knode_u8_alloc();  ((fst_knode_u8 *)res)->value = strtoul (utf8, NULL, 0); break;
        case FST_KNODE_U16  : res = fst_knode_u16_alloc(); ((fst_knode_u16*)res)->value = strtoul (utf8, NULL, 0); break;
        case FST_KNODE_U32  : res = fst_knode_u32_alloc(); ((fst_knode_u32*)res)->value = strtoul (utf8, NULL, 0); break;
        case FST_KNODE_U64  : res = fst_knode_u64_alloc(); ((fst_knode_u64*)res)->value = strtoull(utf8, NULL, 0); break;
        case FST_KNODE_F32  : res = fst_knode_f32_alloc(); ((fst_knode_f32*)res)->value = strtof  (utf8, NULL);    break;
        case FST_KNODE_F64  : res = fst_knode_f64_alloc(); ((fst_knode_f64*)res)->value = strtod  (utf8, NULL);    break;
        }
        if(t != FST_KNODE_STR32)
            free(utf8);
        break; 

    case FST_KNODE_S8   : 
        switch(t) {
            case FST_KNODE_STR8: res=knode_alloc(); free(resval); asprintf(&resval, "%hhd", nval); break;
            case FST_KNODE_STR32: res=knode_alloc(); asprintf(&utf8, "%hhd", nval); resval = realloc(4*(1+strlen(utf8))); utf32toutf8copy(); free(utf8); break;
            default: res=knode_alloc(); resval = nval; break;
        }
        break;
    case FST_KNODE_S16  : break;
    case FST_KNODE_S32  : break;
    case FST_KNODE_S64  : break;
    case FST_KNODE_U8   : break;
    case FST_KNODE_U16  : break;
    case FST_KNODE_U32  : break;
    case FST_KNODE_U64  : break;
    case FST_KNODE_F32  : break;
    case FST_KNODE_F64  : break;
    }

    /* TODO set attributes */

    return res;
}
