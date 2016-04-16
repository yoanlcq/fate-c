/* Initial code */

#include <stdio.h>
#include <fate/tracer.h>

const char *bar(const char *str) { return str; }

int main(void) {
    int cnt = 5 | printf("foo%s\n", bar("baz"));
    return 0;
}

/* After running tracer */
#include <stdio.h>
#include <fate/tracer.h>

const char *bar(const char *str) { return str; }

int main(void) {
    int cnt;
    typeof(bar("baz")) ____wtf1;
    typeof(bar("baz")) ____wtf2;
    typeof(bar("baz")) ____wtf3;
    typeof(printf("foo%s\n", bar("baz"))) ____wtf0;

    ____wtf1 = bar("baz");
    ____wtf2 = bar("baz");
    ____wtf3 = bar("baz");
    FATE_TRACE_4(____wtf0, printf, "foo%s%s%s\n", ____wtf0, ____wtf1, ____wtf2);
    cnt = 5 | ____wtf0;

    return 0;
}

/* When FATE_TRACER_ENABLED is defined */
/* 
 *  Comment mon truc gère les Macros ??
 *  Lancer le C préprocesseur une fois;
 *  Ecrire dans le fichier.
 *  Puis gcc est réappelé sur ce fichier.
 *
 * Toutes les fonctions sont des pointeurs de fonctions qui pointent 
 * initialement sur une fonction vide.
 *
 * $ ./tracer --included=include_list.txt myfile.c
 * $ ./tracer --excluded=exclude_list.txt myfile.c
 * $ ./tracer myfile.c
 *
 * The txt files may contain, for example :
 * annoying_function
 * gl*
 *
 * These are useful for statistics.
 * But for actual debugging, we may want to trace only a few function calls.
 * TRACED is a keyword recognized by tracer, which may be used before
 * a function declaration, or before a function call.
 *
 */

/*
#include <stdio.h>
#include <fate/tracer.h>

const char *bar(const char *str) { return str; }

int main(void) {

    int cnt;
    typeof(bar("baz")) ____wtf;

    fate_logf_trace(
        "~> [%3u] %s:%5u: %s ==> %s(", 
        fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(bar));
    FATE_TRACE_GENERIC("baz");
    fate_logf_trace(
        ")\n~> [%3u] %s:%5u: %s <== ", 
        fate_tracer_level, __FILE__, __LINE__, __func__);
    FATE_TRACE_GENERIC(____wtf = bar("baz"));
    fate_logf_trace("\n");

    fate_logf_trace(
        "~> [%3u] %s:%5u: %s ==> %s(", 
        fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(printf));
    FATE_TRACE_GENERIC("foo%s\n");
    fate_logf_trace(", ");
    FATE_TRACE_GENERIC(____wtf);
    fate_logf_trace(
        ")\n~> [%3u] %s:%5u: %s <== ", 
        fate_tracer_level, __FILE__, __LINE__, __func__);
    FATE_TRACE_GENERIC(cnt = printf("foo\n", ____wtf));
    fate_logf_trace("\n");

    return 0;
}
*/
