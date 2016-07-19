#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h> /* usleep() */
#include <fcntl.h>
#include <string.h> /* memset() */
#include "fe_linuxperf.h"

static void visit(const char *name, fe_linuxperf_state_counter *c) {
    if(c->value) 
        printf("%-36s: %"PRIu64" [*%"PRIu64"/%"PRIu64"]\n", 
               name, c->value, c->time_enabled, c->time_running);
    //else    printf("%-36s: <zero>\n", name);
}

void do_some_work(void) {
    const size_t size = 1<<20;
    char *foo = malloc(size);
    if(!foo) {
        fputs("Whoops, no space left on the heap.\n", stderr);
        exit(EXIT_FAILURE);
    }
    memset(foo, 42, size);
    int fd = creat("/tmp/foobar.bin", 0666);
    write(fd, foo, size);
    close(fd);
    free(foo);
}

int main(int argc, char *argv[]) {
    fe_linuxperf pc;
    fe_linuxperf_state res;
    fe_linuxperf_setup();
    fe_linuxperf_init(&pc, -1);
    for(;;) {
        printf("\033c");
        fe_linuxperf_restart(&pc, 1);
        do_some_work();
        fe_linuxperf_retrieve(&pc, &res, 1);
        fe_linuxperf_state_visit(&res, visit);
        usleep(1000000);
    }
    fe_linuxperf_deinit(&pc);
    return EXIT_SUCCESS;
}
