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

unsigned fe_tracer_level = 0;

#if __STDC_VERSION__ >= 201112L

void fe_trace_chr(const int c) {
    switch(c) {
    case '\a': fe_logf_trace("\\a");  break;
    case '\b': fe_logf_trace("\\b");  break;
    case '\f': fe_logf_trace("\\f");  break;
    case '\n': fe_logf_trace("\\n");  break;
    case '\r': fe_logf_trace("\\r");  break;
    case '\t': fe_logf_trace("\\t");  break;
    case '\v': fe_logf_trace("\\v");  break;
    case '\\': fe_logf_trace("\\\\"); break;
    case '\'': fe_logf_trace("'");    break;
    case '\"': fe_logf_trace("\"");   break;
    case '\?': fe_logf_trace("?");    break;
    case '\0': fe_logf_trace("\\0");  break;
    default: 
        if(isprint(c)) fe_logf_trace("%c", c);
        else fe_logf_trace("\\x%2hhx", (char) c);
        break;
    }
}
void fe_trace_sc(const signed char c) {
    if(c==EOF) {
        fe_logf_trace("EOF");
        return;
    }
    fe_logf_trace("'");
    fe_trace_chr(c);
    fe_logf_trace("'");
}
void fe_trace_str(const signed char *str) {
    if(str==NULL) {
        fe_logf_trace("NULL");
        return;
    }
    fe_logf_trace("\"");
    for(; *str ; ++str)
        fe_trace_chr(*str);
    fe_logf_trace("\"");
}
void fe_trace_ss(const signed short c) {
    fe_logf_trace("%hd", c);
    if(isprint(c)) {
        fe_logf_trace(" ('"); 
        fe_trace_chr(c); 
        fe_logf_trace("')");
    }
}
void fe_trace_us(const unsigned short c) {
    fe_logf_trace("%hu", c);
    if(isprint((int)c)) {
        fe_logf_trace(" ('"); 
        fe_trace_chr(c); 
        fe_logf_trace("')");
    }
}
void fe_trace_s(const signed int c) {
    fe_logf_trace("%d", c);
    if(isprint(c)) {
        fe_logf_trace(" ('"); 
        fe_trace_chr(c); 
        fe_logf_trace("')");
    }
}
void fe_trace_u(const unsigned int c) {
    fe_logf_trace("%u", c);
    if(isprint((int)c)) {
        fe_logf_trace(" ('"); 
        fe_trace_chr(c); 
        fe_logf_trace("')");
    }
}
void fe_trace_sl(const signed long c) {
    fe_logf_trace("%ld", c);
}
void fe_trace_ul(const unsigned long c) {
    fe_logf_trace("%lu", c);
}
void fe_trace_sll(const signed long long c) {
    fe_logf_trace("%lld", c);
}
void fe_trace_ull(const unsigned long long c) {
    fe_logf_trace("%llu", c);
}
void fe_trace_f(const float c) {
    fe_logf_trace("%f", c);
}
void fe_trace_d(const double c) {
    fe_logf_trace("%lf", c);
}
void fe_trace_ld(const long double c) {
    fe_logf_trace("%Lf", c);
}
void fe_trace_default(const void *p) {
    if(p==NULL) fe_logf_trace("NULL");
    else fe_logf_trace("%p", p);
}
void fe_trace_voidp(const void *p) {
    if(p==NULL) fe_logf_trace("NULL");
    else fe_logf_trace("(void*) %p", p);
}

#endif /* __STDC_VERSION__ */
