
void fe_mt_mutex_lock_x86rtm(fe_mt_mutex *mutex) {

    /* Currently, we prevent nested transactions. 
     * FIXME Prove it is actually required. */
    if(_xtest()) 
        return;

retry:
    unsigned status = _xbegin();
    if(status == _XBEGIN_STARTED) {
#ifdef FE_MT_MUTEX_CHECK_DEADLOCKS
        fe_mt_mutex_check_lockable(mutex);
#endif
        return;
    }
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

#ifdef FE_MT_MUTEX_TRY_X86HLE
    fe_mt_mutex_lock_x86hle(mutex);
#else
    fe_mt_mutex_lock_real(mutex);
#endif
}

void fe_mt_mutex_unlock_x86rtm(fe_mt_mutex *mutex) {
    if(_xtest()) {
        _xend();
#ifdef FE_MT_MUTEX_CHECK_DEADLOCKS
        fe_mt_mutex_check_unlockable(mutex);
#endif
        return;
    }
#ifdef FE_MT_MUTEX_TRY_X86HLE
    fe_mt_mutex_unlock_x86hle(mutex);
#else
    fe_mt_mutex_unlock_real(mutex);
#endif
}


