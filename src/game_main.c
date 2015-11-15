#include <fate/fate.h>

fate_globalstate gbs;

int main(int argc, char *argv[]) {
    fate_globalstate_init(&gbs);
    /* Parse launchtime cfg */
    /* Parse commandline options */
    /* Setup crash handler */
    /* Init VFS and root node. */
    fate_globalstate_deinit(&gbs);
    return EXIT_SUCCESS;
}
