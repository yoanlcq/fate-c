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


static void fe_mt_tsx_setup(void); /* Defined far below */

static fe_mt_threadid main_id = 0;

void          fe_mt_setup(void) {
    fe_mt_mutex_init(&pool_mutex);
    pool.slots = fe_mem_heapalloc(1, fe_mt_threadpool_slot, "Thread pool");
    pool.count = 1;
    main_id = fe_mt_get_self_id();
    fe_mt_tsx_setup();
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
void          fe_mt_set_self_priority(fe_mt_threadpriority prio) {
    SDL_SetThreadPriority(prio);
}

fe_mt_threadid          fe_mt_get_main_id(void) {
    return main_id;
}
fe_mt_threadid          fe_mt_get_self_id(void) {
    return SDL_ThreadID();
}

fe_mt_thread  fe_mt_thread_create(int (*func)(void*), const char *name, void *arg) {
    SDL_Thread *th = SDL_CreateThread(func, name, arg);
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
fe_mt_threadid           fe_mt_thread_get_id(fe_mt_thread t) {
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
bool fe_mt_mutex_try_lock(fe_mt_mutex *mutex) {
    return !SDL_TryLockMutex(*mutex);
}
void fe_mt_mutex_unlock(fe_mt_mutex *mutex) {
    SDL_UnlockMutex(*mutex);
}



bool fe_mt_spinlock_try_lock(fe_mt_spinlock *spinlock) {
    return SDL_AtomicTryLock(spinlock);
}
void fe_mt_spinlock_lock    (fe_mt_spinlock *spinlock) {
    SDL_AtomicLock(spinlock);
}
void fe_mt_spinlock_unlock  (fe_mt_spinlock *spinlock) {
    SDL_AtomicUnlock(spinlock);
}



void fe_mt_cond_init     (fe_mt_cond *cond) { *cond = SDL_CreateCond(); }
void fe_mt_cond_deinit   (fe_mt_cond *cond) { SDL_DestroyCond(*cond); }
void fe_mt_cond_wait     (fe_mt_cond *cond, 
                          fe_mt_mutex *mutex, 
                          uint32_t timeout_milliseconds)
{
    SDL_CondWaitTimeout(*cond, *mutex, timeout_milliseconds);
}
void fe_mt_cond_signal   (fe_mt_cond *cond) { SDL_CondSignal(*cond); }
void fe_mt_cond_broadcast(fe_mt_cond *cond) { SDL_CondBroadcast(*cond); }




#if defined(FE_MT_TSX_TRY_X86HLE)
static void fe_mt_tsx_init_dummy(fe_mt_tsx *x) {}
#endif
static void fe_mt_tsx_init_fallback(fe_mt_tsx *x) {
    fe_mt_mutex_init(&x->as_mutex);
}
static void fe_mt_tsx_deinit_fallback(fe_mt_tsx *x) {
    fe_mt_mutex_deinit(&x->as_mutex);
}
static void fe_mt_tsx_lock_fallback(fe_mt_tsx *x) {
    fe_mt_mutex_lock(&x->as_mutex);
}
static void fe_mt_tsx_unlock_fallback(fe_mt_tsx *x) {
    fe_mt_mutex_unlock(&x->as_mutex);
}
void (*fe_mt_tsx_init)  (fe_mt_tsx *t) = fe_mt_tsx_init_fallback;
void (*fe_mt_tsx_deinit)(fe_mt_tsx *t) = fe_mt_tsx_deinit_fallback;
void (*fe_mt_tsx_lock)  (fe_mt_tsx *t) = fe_mt_tsx_lock_fallback;
void (*fe_mt_tsx_unlock)(fe_mt_tsx *t) = fe_mt_tsx_unlock_fallback;


#if defined(FE_MT_TSX_TRY_X86HLE)

#ifdef _MSVC_VER
#pragma intrinsic(_InterlockedCompareExchange, _InterlockedExchange)
#endif

static void fe_mt_tsx_lock_x86hle(fe_mt_tsx *tsx) {
#ifdef _MSVC_VER
    /* See https://msdn.microsoft.com/en-us/library/ttk2z1ws.aspx
     * MS does not place an _mm_pause() in that busy wait here, unlike 
     * GCC does. I chose to add it anyway, but it should be benchmarked. */
    while(_InterlockedCompareExchange_HLEAcquire(&(tsx->as_hle), 1, 0))
        fe_hw_mm_pause();
#else
    while(__atomic_exchange_4(&(tsx->as_hle), 1, __ATOMIC_ACQUIRE
                                               | __ATOMIC_HLE_ACQUIRE)) 
        fe_hw_mm_pause();
#endif
}

static void fe_mt_tsx_unlock_x86hle(fe_mt_tsx *tsx) {
#ifdef _MSVC_VER
    _InterlockedExchange_HLERelease(&(tsx->as_hle), 0);
#else
    __atomic_store_4(&(tsx->as_hle), 0, __ATOMIC_RELEASE 
                                      | __ATOMIC_HLE_RELEASE);
#endif
}

#endif /* FE_MT_TSX_TRY_X86HLE */




#if defined(FE_MT_TSX_TRY_X86RTM)

static void fe_mt_tsx_lock_x86rtm(fe_mt_tsx *tsx) {

    static const char *TAG = "fe_mt";

    /* Currently, we prevent nested transactions. 
     * FIXME Prove it is actually required. */
    if(_xtest()) 
        return;

retry:
    unsigned status = _xbegin();
    if(status == _XBEGIN_STARTED)
        return;
    if(status & _XABORT_RETRY)
        goto retry;

    fe_logv("x86 RTM transaction aborted."
              "Falling back to regular lock. Reason :\n\t");
    if(status & _XABORT_EXPLICIT)
        fe_logv(TAG, "Explicit abort. Code : %d\n", _XABORT_CODE(status)); 
    if(status & _XABORT_CONFLICT)
        fe_logv(TAG, "Memory conflict with another thread.\n");
    if(status & _XABORT_CAPACITY)
        fe_logv(TAG, "Too much memory used by the transaction.\n");
    if(status & _XABORT_DEBUG)
        fe_logv(TAG, "Debug trap.\n");
    if(status & _XABORT_NESTED)
        fe_logv(TAG, "Abort in an inner nested transaction.\n");

    fe_mt_tsx_lock_fallback(tsx);
}

static void fe_mt_tsx_unlock_x86rtm(fe_mt_tsx *tsx) {
    if(_xtest()) {
        _xend();
        return;
    }
    fe_mt_tsx_unlock_fallback(tsx);
}


#endif /* FE_MT_TSX_TRY_X86RTM */



static void fe_mt_tsx_setup(void) {
#if defined(FE_MT_TSX_TRY_X86HLE)
    if(fe_hw_x86_cpu_info.has_hle) {
        fe_mt_tsx_init   = fe_mt_tsx_init_dummy;
        fe_mt_tsx_deinit = fe_mt_tsx_init_dummy;
        fe_mt_tsx_lock   = fe_mt_tsx_lock_x86hle;
        fe_mt_tsx_unlock = fe_mt_tsx_unlock_x86hle;
    }
#endif
#if defined(FE_MT_TSX_TRY_X86RTM)
    if(fe_hw_x86_cpu_info.has_rtm) {
        fe_mt_tsx_init   = fe_mt_tsx_init_fallback;
        fe_mt_tsx_deinit = fe_mt_tsx_init_fallback;
        fe_mt_tsx_lock   = fe_mt_tsx_lock_x86rtm;
        fe_mt_tsx_unlock = fe_mt_tsx_unlock_x86rtm;
    }
#endif
}
