#ifndef FST_SCHEDULER_H
#define FST_SCHEDULER_H

#include <stdbool.h>

struct fst_scheduler {
    int exit_status;
};
typedef struct fst_scheduler fst_scheduler;

inline void fst_scheduler_init         (fst_scheduler *s);
inline void fst_scheduler_deinit       (fst_scheduler *s);
/*  */ bool fst_scheduler_next_frame   (fst_scheduler *s);
inline void fst_scheduler_handle_event (fst_scheduler *s, sfEvent e);
inline int  fst_scheduler_exit_status  (fst_scheduler *s);

#endif /* FST_SCHEDULER_H */
