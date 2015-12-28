#ifndef FATE_DEFS_COMPILER
#define FATE_DEFS_COMPILER

#if __STDC_VERSION__ < 199901L
#define __func__ ""
#endif

#ifndef __GNUC__
#define __builtin_prefetch(addr, ...)
#define __builtin_return_address(level)
#define __attribute__(foo)
#endif

#endif /* FATE_DEFS_COMPILER */
