#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#ifdef __GNUC__
#include <execinfo.h>
#endif

/*
 * See signal(7), sigaction(2).
 */

#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE

#if _POSIX_C_SOURCE >= 199309L
void fate_sigaction(int signum, siginfo_t *siginfo, void *ucontext)
#else
void fate_sighandler(int signum)
#endif
{
#define FATE_BT_SIZE 128
    void *bt_buffer[FATE_BT_SIZE];
    char **bt_strings;
    int bt, fatal;
    unsigned i;
    int bt_nptrs;


    fate_logf_err("Program received ");

#define HELPER(S_, A_, B_, C_) \
    case S_: fate_logf_err(#S_ ", " A_); B_; C_; break;
    switch(signum) {
    HELPER(SIGHUP,  "death of controlling process",  bt=0, fatal=0);
    HELPER(SIGINT,  "interrupt from keyboard",       bt=0, fatal=1);
    HELPER(SIGQUIT, "quit from keyboard",            bt=0, fatal=1);
    HELPER(SIGTERM, "termination",                   bt=0, fatal=1); 
    HELPER(SIGILL,  "illegal instruction",           bt=0, fatal=1); 
    HELPER(SIGPIPE, "broken pipe",                   bt=0, fatal=1); 
    HELPER(SIGFPE,  "floating point exception",      bt=1, fatal=1);
    HELPER(SIGBUS,  "bus error (bad memory access)", bt=1, fatal=1);
    HELPER(SIGSEGV, "invalid memory reference",      bt=1, fatal=1);
    HELPER(SIGABRT, "abort",                         bt=1, fatal=1);
    default: fate_logf_err("signal %d", signum);     bt=1, fatal=0; break;
    }
#undef HELPER
#if _POSIX_C_SOURCE >= 199309L
    /* Below, the strings are separated into keywords so the compiler reuses
     * them instead of storing, each time, a long string. See GCC's manual. */
#define HELPER(S_, A_, B_, C_) \
    case S_: fate_logf_err(" (%s%s%s)", A_, B_, C_); break;
    switch(siginfo->si_code) {
    HELPER(SI_USER,     "sent by ", "user", ""); 
    HELPER(SI_KERNEL,   "sent by ", "kernel", ""); 
    HELPER(FPE_INTDIV,  "integer ", "divide by zero", "");
    HELPER(FPE_INTOVF,  "integer ", "over", "flow");
    HELPER(FPE_FLTDIV,  "floating-point ", "divide by zero", "");
    HELPER(FPE_FLTOVF,  "floating-point ", "over", "flow");
    HELPER(FPE_FLTUND,  "floating-point ", "under", "flow");
    HELPER(FPE_FLTRES,  "floating-point ", "inexact result", "");
    HELPER(FPE_FLTINV,  "floating-point ", "invalid ", "operation");
    HELPER(FPE_FLTSUB,  "subscript out of range", "", "");
    HELPER(SEGV_MAPERR, "address", " not mapped to ", "object");
    HELPER(SEGV_ACCERR, "invalid ", "permissions for mapped ", "object");
    HELPER(BUS_ADRALN,  "invalid ", "address", " alignment");
    HELPER(BUS_ADRERR,  "nonexistent physical ", "address", "");
    HELPER(BUS_OBJERR,  "object", "-specific hardware error", "");
    }
#undef HELPER
    fate_logf_err(".\n");
#ifdef __GNUC__
    if(bt) {
        bt_nptrs = backtrace(bt_buffer, FATE_BT_SIZE);
        bt_strings = backtrace_symbols(bt_buffer, bt_nptrs);
        if(!bt_strings)
            perror("backtrace_symbols");
        for(i=0; i<bt_nptrs; ++i)
            fate_logf_err("%s\n", bt_strings[i]);
        free(strings);
    }
#endif
    /* Deinit global state */
    fate_globalstate_deinit(&fate_globalstate_instance);
    if(term)
        exit(EXIT_FAILURE);
}

void install_posix_sighandler(void) {
    struct sigaction sa;

    sa.sa_mask = 0;
    sa.sa_flags = SA_RESTART;
#if _POSIX_C_SOURCE >= 199309L
    sa.sa_flags |= SA_SIGACTION;
    sa.sa_sigaction = &fate_sigaction;
#else
    sa.sa_handler = &fate_sighandler;
#endif
    sigaction(SIGHUP,  &sa, NULL);
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGILL,  &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL);
    sigaction(SIGFPE,  &sa, NULL);
    sigaction(SIGBUS,  &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
}

#else  /* _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

void install_posix_sighandler(void) {
    fputs("The POSIX signal handler was not available "
          "at compilation.\n", stderr);
}

#endif /* _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */
