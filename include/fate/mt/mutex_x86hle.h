    typedef union {
        SDL_mutex *sdl_mutex;
    #ifdef FATE_MT_MUTEX_CHECK_DEADLOCKS
        long       id;
    #endif
        long       hle_lock;
    } fate_mt_mutex;
#else
    #ifdef FATE_MT_MUTEX_CHECK_DEADLOCKS
        typedef struct {
            SDL_mutex *sdl_mutex;
            long id;
        } fate_mt_mutex;
    #else
        typedef SDL_mutex fate_mt_mutex;
    #endif
#endif

#ifdef _MSVC_VER
#pragma intrinsic(_InterlockedCompareExchange, _InterlockedExchange)
#endif

void (*fate_mt_mutex_lock_x86hle)(fate_mt_mutex *mutex);
void fate_mt_mutex_lock_x86hle_real(fate_mt_mutex *mutex) {
#ifdef FATE_MT_MUTEX_CHECK_DEADLOCKS
    fate_mt_mutex_check_lockable(mutex);
#endif
#ifdef _MSVC_VER
    /* See https://msdn.microsoft.com/en-us/library/ttk2z1ws.aspx
     * MS does not place an _mm_pause() in that busy wait here, unlike 
     * GCC does. I chose to add it anyway, but it should be benchmarked. */
    while(_InterlockedCompareExchange(&(mutex->hle_lock), 1, 0))
        _mm_pause();
#else
    while(__atomic_exchange_n(&(mutex->hle_lock), 1, __ATOMIC_ACQUIRE
                                                   | __ATOMIC_HLE_ACQUIRE)) 
        _mm_pause();
#endif
}

void (*fate_mt_mutex_unlock_x86hle)(fate_mt_mutex *mutex);
void fate_mt_mutex_unlock_x86hle_real(fate_mt_mutex *mutex) {
#ifdef _MSVC_VER
    _InterlockedExchange(&(mutex->hle_lock), 0);
#else
    __atomic_store_n(&(mutex->hle_lock), 0, __ATOMIC_RELEASE 
                                          | __ATOMIC_HLE_RELEASE);
#ifdef FATE_MT_MUTEX_CHECK_DEADLOCKS
    fate_mt_mutex_check_unlockable(mutex);
#endif
}
