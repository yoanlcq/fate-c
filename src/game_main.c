#include <fate/fate.h>

fate_globalstate fate_globalstate_instance;

int main(int argc, char *argv[]) {
    fate_globalstate_init(&fate_globalstate_instance);
    /* FATE 0.1.0 - compiled on Feb 10 2016 */
    /* Parse launchtime cfg */
    /* Parse commandline options */
    /* Setup crash handler */
    /* Init VFS and root node. */
    fate_globalstate_deinit(&fate_globalstate_instance);
    return EXIT_SUCCESS;
}
