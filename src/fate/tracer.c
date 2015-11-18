#include <stdio.h>
#include <ctype.h>
#include <fate/tracer.h>
#include <fate/log.h>

unsigned fate_tracer_level = 0;

#if __STDC_VERSION__ >= 201112L

void fate_trace_chr(const int c) {
    switch(c) {
    case '\a': fate_logf_trace("\\a");  break;
    case '\b': fate_logf_trace("\\b");  break;
    case '\f': fate_logf_trace("\\f");  break;
    case '\n': fate_logf_trace("\\n");  break;
    case '\r': fate_logf_trace("\\r");  break;
    case '\t': fate_logf_trace("\\t");  break;
    case '\v': fate_logf_trace("\\v");  break;
    case '\\': fate_logf_trace("\\\\"); break;
    case '\'': fate_logf_trace("'");    break;
    case '\"': fate_logf_trace("\"");   break;
    case '\?': fate_logf_trace("?");    break;
    case '\0': fate_logf_trace("\\0");  break;
    default: 
        if(isprint(c)) fate_logf_trace("%c", c);
        else fate_logf_trace("\\x%2hhx", (char) c);
        break;
    }
}
void fate_trace_sc(const signed char c) {
    if(c==EOF) {
        fate_logf_trace("EOF");
        return;
    }
    fate_logf_trace("'");
    fate_trace_chr(c);
    fate_logf_trace("'");
}
void fate_trace_str(const signed char *str) {
    if(str==NULL) {
        fate_logf_trace("NULL");
        return;
    }
    fate_logf_trace("\"");
    for(; *str ; ++str)
        fate_trace_chr(*str);
    fate_logf_trace("\"");
}
void fate_trace_ss(const signed short c) {
    fate_logf_trace("%hd", c);
    if(isprint(c)) {
        fate_logf_trace(" ('"); 
        fate_trace_chr(c); 
        fate_logf_trace("')");
    }
}
void fate_trace_us(const unsigned short c) {
    fate_logf_trace("%hu", c);
    if(isprint((int)c)) {
        fate_logf_trace(" ('"); 
        fate_trace_chr(c); 
        fate_logf_trace("')");
    }
}
void fate_trace_s(const signed int c) {
    fate_logf_trace("%d", c);
    if(isprint(c)) {
        fate_logf_trace(" ('"); 
        fate_trace_chr(c); 
        fate_logf_trace("')");
    }
}
void fate_trace_u(const unsigned int c) {
    fate_logf_trace("%u", c);
    if(isprint((int)c)) {
        fate_logf_trace(" ('"); 
        fate_trace_chr(c); 
        fate_logf_trace("')");
    }
}
void fate_trace_sl(const signed long c) {
    fate_logf_trace("%ld", c);
}
void fate_trace_ul(const unsigned long c) {
    fate_logf_trace("%lu", c);
}
void fate_trace_sll(const signed long long c) {
    fate_logf_trace("%lld", c);
}
void fate_trace_ull(const unsigned long long c) {
    fate_logf_trace("%llu", c);
}
void fate_trace_f(const float c) {
    fate_logf_trace("%f", c);
}
void fate_trace_d(const double c) {
    fate_logf_trace("%lf", c);
}
void fate_trace_ld(const long double c) {
    fate_logf_trace("%Lf", c);
}
void fate_trace_default(const void *p) {
    if(p==NULL) fate_logf_trace("NULL");
    else fate_logf_trace("%p", p);
}
void fate_trace_voidp(const void *p) {
    if(p==NULL) fate_logf_trace("NULL");
    else fate_logf_trace("(void*) %p", p);
}

#endif /* __STDC_VERSION__ */
