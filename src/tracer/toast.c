#include <stdio.h>
#define FATE_ENABLE_TRACING
#include <fate/tracer.h>

int recursive(const char *str, int i, double d) {
    int res;
    if(*str=='\0')
        return i+i;
    FATE_TRACE_3(res, recursive, str+1, i+i, d*d);
    return res+i;
}

int main(void) {
    int foo;
    FATE_TRACE_3(foo, recursive, "Tracer", 42, 2.25);
    return foo;
}
