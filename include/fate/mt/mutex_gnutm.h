#define FE_MT_MUTEX_DECL(name) 
#define fe_mt_mutex_init(mutex)
#define fe_mt_mutex_deinit(mutex)
#define fe_mt_mutex_lock(mutex)   __transaction_relaxed {
#define fe_mt_mutex_unlock(mutex) }
