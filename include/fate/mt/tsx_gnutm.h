#define FE_MT_TSX_DECL(name) 
#define fe_mt_tsx_init(mutex)
#define fe_mt_tsx_deinit(mutex)
#define fe_mt_tsx_lock(mutex)   __transaction_relaxed {
#define fe_mt_tsx_unlock(mutex) }
