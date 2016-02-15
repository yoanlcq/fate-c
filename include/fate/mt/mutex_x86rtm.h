
void fate_mt_mutex_lock_x86rtm(fate_mt_mutex *mutex) {

    /* Currently, we prevent nested transactions. 
     * FIXME Prove it is actually required. */
    if(_xtest()) 
        return;

retry:
    unsigned status = _xbegin();
    if(status == _XBEGIN_STARTED) {
#ifdef FATE_MT_MUTEX_CHECK_DEADLOCKS
        fate_mt_mutex_check_lockable(mutex);
#endif
        return;
    }
    if(status & _XABORT_RETRY)
        goto retry;

    fate_logv("x86 RTM transaction aborted."
              "Falling back to regular lock. Reason :\n\t");
    if(status & _XABORT_EXPLICIT)
        fate_logv(TAG, "Explicit abort. Code : %d\n", _XABORT_CODE(status)); 
    if(status & _XABORT_CONFLICT)
        fate_logv(TAG, "Memory conflict with another thread.\n");
    if(status & _XABORT_CAPACITY)
        fate_logv(TAG, "Too much memory used by the transaction.\n");
    if(status & _XABORT_DEBUG)
        fate_logv(TAG, "Debug trap.\n");
    if(status & _XABORT_NESTED)
        fate_logv(TAG, "Abort in an inner nested transaction.\n");

#ifdef FATE_MT_MUTEX_TRY_X86HLE
    fate_mt_mutex_lock_x86hle(mutex);
#else
    fate_mt_mutex_lock_real(mutex);
#endif
}

void fate_mt_mutex_unlock_x86rtm(fate_mt_mutex *mutex) {
    if(_xtest()) {
        _xend();
#ifdef FATE_MT_MUTEX_CHECK_DEADLOCKS
        fate_mt_mutex_check_unlockable(mutex);
#endif
        return;
    }
#ifdef FATE_MT_MUTEX_TRY_X86HLE
    fate_mt_mutex_unlock_x86hle(mutex);
#else
    fate_mt_mutex_unlock_real(mutex);
#endif
}


