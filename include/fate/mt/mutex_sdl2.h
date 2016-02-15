#define FATE_MT_MUTEX_DECL(name)    SDL_mutex *name;
#define fate_mt_mutex_init(mutex)   mutex = SDL_CreateMutex()
#define fate_mt_mutex_deinit(mutex) SDL_DestroyMutex(mutex)
#define fate_mt_mutex_lock(mutex)   SDL_LockMutex(mutex)
#define fate_mt_mutex_unlock(mutex) SDL_UnlockMutex(mutex)
