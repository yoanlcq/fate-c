#define FATE_MT_MUTEX_DECL(name) 
#define fate_mt_mutex_init(mutex)
#define fate_mt_mutex_deinit(mutex)
#define fate_mt_mutex_lock(mutex)   __transaction_relaxed {
#define fate_mt_mutex_unlock(mutex) }
