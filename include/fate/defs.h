#ifndef FATE_DEFS_H
#define FATE_DEFS_H

#define FATE_VERSION_MAJOR 0
#define FATE_VERSION_MINOR 1
#define FATE_VERSION_PATCH 0
#define FATE_VERSION_CODENAME "Rainbow Dice"

#define XSTRINGIFY(X) STRINGIFY(X)
#define STRINGIFY(X) #X

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

#include <fate/defs_compiler.h>
#include <fate/defs_platform.h>

#include <fate/log.h>

#endif /* FST_DEFS_H */
