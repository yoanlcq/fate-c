#ifdef FATE_MT_MUTEX_CHECK_DEADLOCKS
void fate_mt_mutex_init(fate_mt_mutex *mutex) {
    mutex->sdl_mutex = SDL_CreateMutex();
    static_addnode(&(mutex->id));
}
void fate_mt_mutex_deinit(fate_mt_mutex *mutex) {
    static_removenode(mutex->id);
    SDL_DestroyMutex(mutex->sdl_mutex);
}
void fate_mt_mutex_check_lockable(fate_mt_mutex *mutex) {
    static_connectnode(mutex->id, SDL_ThreadID());
}
void fate_mt_mutex_check_unlockable(fate_mt_mutex *mutex) {
    static_diconnectnode(mutex->id, SDL_ThreadID());
}
#endif

