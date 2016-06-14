/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */

/*! \file fate/tracer.c
 *  \brief Support for tracing functions.
 *
 * TODO
 */

#include <stdio.h>
#include <ctype.h>
#include <fate/tracer.h>
#include <fate/log.h>

#ifdef FE_LOG_USE_VERBOSE
static const char *TAG = "fe_tracer";
#endif

unsigned fe_tracer_level = 0;

#if __STDC_VERSION__ >= 201112L

void fe_trace_chr(const int c) {
    switch(c) {
    case '\a': fe_logv(TAG, "\\a");  break;
    case '\b': fe_logv(TAG, "\\b");  break;
    case '\f': fe_logv(TAG, "\\f");  break;
    case '\n': fe_logv(TAG, "\\n");  break;
    case '\r': fe_logv(TAG, "\\r");  break;
    case '\t': fe_logv(TAG, "\\t");  break;
    case '\v': fe_logv(TAG, "\\v");  break;
    case '\\': fe_logv(TAG, "\\\\"); break;
    case '\'': fe_logv(TAG, "'");    break;
    case '\"': fe_logv(TAG, "\"");   break;
    case '\?': fe_logv(TAG, "?");    break;
    case '\0': fe_logv(TAG, "\\0");  break;
    default: 
        if(isprint(c)) fe_logv(TAG, "%c", c);
        else fe_logv(TAG, "\\x%2hhx", (char) c);
        break;
    }
}
void fe_trace_sc(const signed char c) {
    if(c==EOF) {
        fe_logv(TAG, "EOF");
        return;
    }
    fe_logv(TAG, "'");
    fe_trace_chr(c);
    fe_logv(TAG, "'");
}
void fe_trace_str(const signed char *str) {
    if(str==NULL) {
        fe_logv(TAG, "NULL");
        return;
    }
    fe_logv(TAG, "\"");
    for(; *str ; ++str)
        fe_trace_chr(*str);
    fe_logv(TAG, "\"");
}
void fe_trace_ss(const signed short c) {
    fe_logv(TAG, "%hd", c);
    if(isprint(c)) {
        fe_logv(TAG, " ('"); 
        fe_trace_chr(c); 
        fe_logv(TAG, "')");
    }
}
void fe_trace_us(const unsigned short c) {
    fe_logv(TAG, "%hu", c);
    if(isprint((int)c)) {
        fe_logv(TAG, " ('"); 
        fe_trace_chr(c); 
        fe_logv(TAG, "')");
    }
}
void fe_trace_s(const signed int c) {
    fe_logv(TAG, "%d", c);
    if(isprint(c)) {
        fe_logv(TAG, " ('"); 
        fe_trace_chr(c); 
        fe_logv(TAG, "')");
    }
}
void fe_trace_u(const unsigned int c) {
    fe_logv(TAG, "%u", c);
    if(isprint((int)c)) {
        fe_logv(TAG, " ('"); 
        fe_trace_chr(c); 
        fe_logv(TAG, "')");
    }
}
void fe_trace_sl(const signed long c) {
    fe_logv(TAG, "%ld", c);
}
void fe_trace_ul(const unsigned long c) {
    fe_logv(TAG, "%lu", c);
}
void fe_trace_sll(const signed long long c) {
    fe_logv(TAG, "%lld", c);
}
void fe_trace_ull(const unsigned long long c) {
    fe_logv(TAG, "%llu", c);
}
void fe_trace_f(const float c) {
    fe_logv(TAG, "%f", c);
}
void fe_trace_d(const double c) {
    fe_logv(TAG, "%lf", c);
}
void fe_trace_ld(const long double c) {
    fe_logv(TAG, "%Lf", c);
}
void fe_trace_default(const void *p) {
    if(p==NULL) fe_logv(TAG, "NULL");
    else fe_logv(TAG, "%p", p);
}
void fe_trace_voidp(const void *p) {
    if(p==NULL) fe_logv(TAG, "NULL");
    else fe_logv(TAG, "(void*) %p", p);
}

#endif /* __STDC_VERSION__ */
