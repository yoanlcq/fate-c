#include <fate/mem.h>
#include <fate/mt.h>
#include <SDL2/SDL.h>

typedef struct {
    SDL_Thread *thread;
    bool taken;
    int id;
} fe_mt_threadpool_slot;

/* fe_mt_thread is an index into this pool, or -1 in case of failure. */
typedef struct {
    fe_mt_threadpool_slot *slots;
    size_t count;
} fe_mt_threadpool;

static fe_mt_threadpool pool = {0};
static fe_mt_mutex pool_mutex;

void          fe_mt_setup(void) {
    fe_mt_mutex_init(&pool_mutex);
    pool.slots = fe_mem_heapalloc(1, fe_mt_threadpool_slot, "Thread pool");
    pool.count = 1;
}
void fe_mt_cleanup(void) {
    fe_mt_mutex_deinit(&pool_mutex);
    fe_mem_heapfree(pool.slots);
}
size_t        fe_mt_get_thread_count(void) {
    fe_mt_mutex_lock(&pool_mutex);
    size_t cnt = pool.count;
    fe_mt_mutex_unlock(&pool_mutex);
    return cnt;
}
void          fe_mt_set_self_priority(fe_mt_thread_priority prio) {
    SDL_SetThreadPriority(prio);
}

fe_mt_thread  fe_mt_thread_create(int (*func)(void*), const char *name, void *arg) {
    SDL_thread *th = SDL_CreateThread(func, name, arg);
    if(!th)
        return -1;
    fe_mt_mutex_lock(&pool_mutex);
    size_t i;
    for(i=0 ; i<pool.count ; ++i)
        if(!pool.slots[i].taken)
            goto allocate;
    pool.slots = fe_mem_heaprealloc(pool.slots, i+1, fe_mt_threadpool_slot, "");
    if(!pool.slots) {
        fe_mt_mutex_unlock(&pool_mutex);
        return -1;
    }
allocate:
    pool.slots[i].taken = true;
    pool.slots[i].thread = th;
    pool.slots[i].id = SDL_GetThreadID(th);
    fe_mt_mutex_unlock(&pool_mutex);
    return i;
}
void          fe_mt_thread_detach(fe_mt_thread t) {
    SDL_DetachThread(pool.slots[t].thread);
    fe_mt_mutex_lock(&pool_mutex);
    pool.slots[t].taken = false;
    fe_mt_mutex_unlock(&pool_mutex);
}
int           fe_mt_thread_get_id(fe_mt_thread t) {
    return pool.slots[t].id;
}
const char *  fe_mt_thread_get_name(fe_mt_thread t) {
    return SDL_GetThreadName(pool.slots[t].thread);
}
int           fe_mt_thread_wait(fe_mt_thread t) {
    int status;
    SDL_WaitThread(pool.slots[t].thread, &status);
    fe_mt_mutex_lock(&pool_mutex);
    pool.slots[t].taken = false;
    fe_mt_mutex_unlock(&pool_mutex);
    return status;
}


void fe_mt_mutex_init(fe_mt_mutex *mutex) {
    *mutex = SDL_CreateMutex();
}
void fe_mt_mutex_deinit(fe_mt_mutex *mutex) {
    SDL_DestroyMutex(*mutex);
}
void fe_mt_mutex_lock(fe_mt_mutex *mutex) {
    SDL_LockMutex(*mutex);
}
void fe_mt_mutex_try_lock(fe_mt_mutex *mutex) {
    return !SDL_TryLockMutex(*mutex);
}
void fe_mt_mutex_unlock(fe_mt_mutex *mutex) {
    SDL_UnlockMutex(*mutex);
}

