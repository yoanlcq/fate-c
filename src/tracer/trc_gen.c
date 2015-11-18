#include <stdio.h>

#define PS(X)        puts("    " X " \\")
#define PF(X, ...) printf("    " X " \\\n", __VA_ARGS__)

int main(void) {
    char dict[28] = "TXABCDEFGHIJKLMNOPQRSUVWYZ";
    unsigned x, y;
    for(y=0 ; y<25 ; ++y) {
        printf("    #define %sFATE_TRACE_%u(", y<10 ? " " : "", y);
        for(x=0 ; x<y+2 ; ++x)
            printf("%c%c", dict[x], x<y+1 ? ',' : ')');
        printf(" \\\n                          %c=%c(", dict[0], dict[1]);
        for(x=2 ; x<y+2 ; ++x)
            printf("%c%s", dict[x], x<y+1 ? "," : "");
        puts(")");
    }

    for(y=0 ; y<25 ; ++y) {
        printf("    #define FATE_TRACE_%u(", y);
        for(x=0 ; x<y+2 ; ++x)
            printf("%c%c", dict[x], x<y+1 ? ',' : ')');
        puts(" \\");
        /* Rationale : What if the function returns early ? */
        PS("    ++fate_tracer_level;");
        PS("    fate_logf_trace(");
        PS("        \"~> [%3u] %s:%5u: %s ==> %s(\",");
        PS("        fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X));");

        for(x=2 ; x<y+2 ; ++x) {
            printf("        FATE_TRACE_GENERIC(%c); \\\n", dict[x]);
            if(x<y+1)
                printf("        fate_logf_trace(\", \"); \\\n");
        }

        PS("    fate_logf_trace(\")\\n\");");

        printf("        %c = %c(", dict[0], dict[1]);
        for(x=2 ; x<y+2 ; ++x)
            printf("%c%s", dict[x], x<y+1 ? "," : "");
        puts("); \\");


        PS("    fate_logf_trace(");
        PS("        \"~> [%3u] %s:%5u: %s <== \",");
        PS("        fate_tracer_level, __FILE__, __LINE__, __func__);");
        PF("    FATE_TRACE_GENERIC(%c);", dict[0]);

        PS("    fate_logf_trace(\"\\n\");");
        puts("        --fate_tracer_level\n");
    }
    return 0;
}
