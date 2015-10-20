#ifndef FST_IMPLEMENTATION_H
#define FST_IMPLEMENTATION_H

#include <fst/scheduler.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>

struct fst_implementation {
    sfContext *dummy_context;
    sfContextSettings *context_settings;
    sfWindow *window;
    fst_scheduler scheduler;
    fst_nodesystem nodesystem;
};
typedef struct fst_implementation fst_implementation;

inline int fst_implementation_init(fst_implementation *impl);
inline int fst_implementation_exec(fst_implementation *impl);

#endif /* FST_IMPLEMENTATION_H */
