/*
 * Have custom glX* function pointers;
 * Select a libGL.so.1 with dlopen();
 *     ls -l $(find /usr/lib* /usr/local/lib* -name libGL.so*)
 * Determine if it is software by looking up glX*MESA functions;
 *     strings libGL.so.1 | grep MESA
 * Redirect custom glX* function pointers to the libGL.so.1's glX functions;
 * Redirect custom glXGetProcAddress to a dlsym() wrapper;
 * Init GLEW;
 * Profit !
 *
 * Some steps may require the user's help.
 * Problems may arise when trying to switch to a hardware context.
 */

#include <glxew.h>


