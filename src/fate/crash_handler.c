#ifdef FATE_DEFS_WINDOWS

#include <Windows.h>
#include <fate/log.h>

/* See http://spin.atomicobject.com/2013/01/13/exceptions-stack-traces-c/ */
LONG WINAPI fate_win32_exception_handler(EXCEPTION_POINTERS *ep)
{
    fate_logf_err("Process received ");
#define HELPER(_S) \
    case _S: fate_logf_err(#_S); break
#define DEFAULT \
    default: fate_logf_err("Unrecognized exception"); break
    switch(ep->ExceptionRecord->ExceptionCode) {
    HELPER(EXCEPTION_ACCESS_VIOLATION);
    HELPER(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
    HELPER(EXCEPTION_BREAKPOINT);
    HELPER(EXCEPTION_DATATYPE_MISALIGNMENT);
    HELPER(EXCEPTION_FLT_DENORMAL_OPERAND);
    HELPER(EXCEPTION_FLT_DIVIDE_BY_ZERO);
    HELPER(EXCEPTION_FLT_INEXACT_RESULT);
    HELPER(EXCEPTION_FLT_INVALID_OPERATION);
    HELPER(EXCEPTION_FLT_OVERFLOW);
    HELPER(EXCEPTION_FLT_STACK_CHECK);
    HELPER(EXCEPTION_FLT_UNDERFLOW);
    HELPER(EXCEPTION_ILLEGAL_INSTRUCTION);
    HELPER(EXCEPTION_IN_PAGE_ERROR);
    HELPER(EXCEPTION_INT_DIVIDE_BY_ZERO);
    HELPER(EXCEPTION_INT_OVERFLOW);
    HELPER(EXCEPTION_INVALID_DISPOSITION);
    HELPER(EXCEPTION_NONCONTINUABLE_EXCEPTION);
    HELPER(EXCEPTION_PRIV_INSTRUCTION);
    HELPER(EXCEPTION_SINGLE_STEP);
    HELPER(EXCEPTION_STACK_OVERFLOW);
    DEFAULT;
    }
#undef HELPER
#undef DEFAULT
    fate_logf_err(".\r\n");

    if(ep->ExceptionRecord->ExceptionCode != EXCEPTION_STACK_OVERFLOW)
        fate_log_stacktrace(&fate_logf_err);

    return EXCEPTION_EXECUTE_HANDLER;
}
 
void fate_install_crash_handler(void) {
    SetUnhandledExceptionFilter(&fate_win32_exception_handler);
}



#else /* Unices */


#include <stdlib.h>
#include <signal.h>
#include <fate/log.h>

#if !(_POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE)

/* Very unlikely, but since it is most often compiled out, it causes no harm. */
void fate_install_crash_handler(void) {
    fate_logf_err("The POSIX signal handler was not available "
                  "at compilation.\n", stderr);
}

#else  /* _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#if _POSIX_C_SOURCE >= 199309L
void fate_sigaction(int signum, siginfo_t *siginfo, void *ucontext)
#else
void fate_sighandler(int signum)
#endif
{
    int btrace, fatal;

    fate_logf_err("Process received ");

#define HELPER(S_, A_, B_, C_) \
    case S_: fate_logf_err(#S_ ": " A_); B_; C_; break
    switch(signum) {
    HELPER(SIGHUP,  "death of controlling process",  btrace=0, fatal=0);
    HELPER(SIGINT,  "interrupt from keyboard",       btrace=0, fatal=1);
    HELPER(SIGQUIT, "quit from keyboard",            btrace=0, fatal=1);
    HELPER(SIGTERM, "termination",                   btrace=0, fatal=1); 
    HELPER(SIGPIPE, "broken pipe",                   btrace=0, fatal=1); 
    HELPER(SIGFPE,  "floating point exception",      btrace=1, fatal=1);
    HELPER(SIGBUS,  "bus error (bad memory access)", btrace=1, fatal=1);
    HELPER(SIGSEGV, "invalid memory reference",      btrace=1, fatal=1);
    HELPER(SIGABRT, "abort",                         btrace=1, fatal=1);
    default: fate_logf_err("signal #%d", signum);    btrace=1, fatal=0; break;
    }
#undef HELPER

#if _POSIX_C_SOURCE >= 199309L

#define HELPER(S_, A_, B_, C_) \
    case S_: fate_logf_err(" (%s%s%s)", A_, B_, C_); break
#define DEFAULT \
    default: fate_logf_err(" (siginfo->si_code = %d)", siginfo->si_code); break

    /* Below, the strings are separated into keywords so the compiler reuses
     * them instead of storing, each time, a long string. See GCC's manual. */

    switch(signum) {
    case SIGFPE:
        switch(siginfo->si_code) {
        HELPER(FPE_INTDIV,  "integer ", "divide by zero", "");
        HELPER(FPE_INTOVF,  "integer ", "over", "flow");
        HELPER(FPE_FLTDIV,  "floating-point ", "divide by zero", "");
        HELPER(FPE_FLTOVF,  "floating-point ", "over", "flow");
        HELPER(FPE_FLTUND,  "floating-point ", "under", "flow");
        HELPER(FPE_FLTRES,  "floating-point ", "inexact result", "");
        HELPER(FPE_FLTINV,  "floating-point ", "invalid ", "operation");
        HELPER(FPE_FLTSUB,  "subscript out of range", "", "");
        DEFAULT;
        }
        break;
    case SIGSEGV:
        switch(siginfo->si_code) {
        HELPER(SEGV_MAPERR, "address", " not mapped to ", "object");
        HELPER(SEGV_ACCERR, "invalid ", "permissions for mapped ", "object");
        DEFAULT;
        }
        break;
    case SIGBUS:
        switch(siginfo->si_code) {
        HELPER(BUS_ADRALN,  "invalid ", "address", " alignment");
        HELPER(BUS_ADRERR,  "nonexistent physical ", "address", "");
        HELPER(BUS_OBJERR,  "object", "-specific hardware error", "");
        DEFAULT;
        }
        break;
    default: 
        switch(siginfo->si_code) {
        HELPER(SI_USER,     "sent by ", "user", ""); 
        HELPER(SI_KERNEL,   "sent by ", "kernel", ""); 
        DEFAULT;
        }
        break;
    }

#undef HELPER
#undef DEFAULT
#endif /* _POSIX_C_SOURCE >= 199309L */

    fate_logf_err(".\n");
    if(btrace)
        fate_log_stacktrace(&fate_logf_err);

    //fate_globalstate_deinit(&fate_globalstate_instance);

    if(fatal)
        exit(EXIT_FAILURE);
}

void fate_install_crash_handler(void) {
    struct sigaction sa = {0};

    sa.sa_flags = SA_RESTART;
#if _POSIX_C_SOURCE >= 199309L
    sa.sa_flags |= SA_SIGINFO;
    sa.sa_sigaction = &fate_sigaction;
#else
    sa.sa_handler = &fate_sighandler;
#endif
    sigaction(SIGHUP,  &sa, NULL);
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL);
    sigaction(SIGFPE,  &sa, NULL);
    sigaction(SIGBUS,  &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
}

#endif /* _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */


#endif /* WINDOWS */


unsigned recursive(unsigned d) {
    unsigned foo = 100/(d--);
    return recursive(d);
}

int main(void) {
    fate_install_crash_handler();
    fate_logf_err("%u\n", recursive(5));
    return 0;
}

