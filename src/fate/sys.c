#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fate/defs.h>
#include <fate/globalstate.h>
#include <fate/sys.h>
#include <fate/log.h>

#if defined(FATE_DEFS_WINDOWS)
#include <Windows.h>
#include <DbgHelp.h>
#elif defined(FATE_DEFS_LINUX)
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/auxv.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#elif defined(FATE_DEFS_FREEBSD)
#include <limits.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#elif defined(FATE_DEFS_OSX)
#include <limits.h>
#include <signal.h>
#include <mach-o/dyld.h>
#include <execinfo.h>
#endif

/*
 *
 *
 *
 *
 *
 *
 *
 *    fate_sys_file_exists()
 *
 *
 *
 *
 *
 *
 *
 */


#if defined(FATE_DEFS_WINDOWS)
inline bool fate_sys_file_exists(const char *path) {
    return GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
}
#else 
inline bool fate_sys_file_exists(const char *path) {
    return !access(path, F_OK);
}
#endif


/*
 *
 *
 *
 *
 *
 *
 *
 *    fate_sys_get_last_write_time()
 *
 *
 *
 *
 *
 *
 *
 */


#if defined(FATE_DEFS_WINDOWS)
inline uint64_t fate_sys_get_last_write_time(const char *path) {
    FILETIME ft;
    HANDLE fh;
    fh = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(fh==INVALID_HANDLE_VALUE) {
        fate_logf_err("Could not stat \"%s\".\n", path);
        return 0;
    }
    GetFileTime(fh, NULL, NULL, &ft);
    CloseHandle(fh);
    return (((uint64_t)ft.dwHighDateTime)<<32)+(uint64_t)ft.dwLowDateTime;
}
#else
inline uint64_t fate_sys_get_last_write_time(const char *path) {
    struct stat st;
    if(stat(path, &st)) {
        fate_logf_err("Could not stat \"%s\".\n", path);
        return 0;
    }
    return st.st_mtime;
}
#endif

/*
 *
 *
 *
 *
 *
 *
 *
 *    fate_set_current_directory()
 *
 *
 *
 *
 *
 *
 *
 */


#if defined(FATE_DEFS_WINDOWS)
inline bool fate_sys_set_current_directory(const char *path) {
    return SetCurrentDirectory(path);
}
#else
inline bool fate_sys_set_current_directory(const char *path) {
    return !chdir(path);
}
#endif



/*
 *
 *
 *
 *
 *
 *
 *
 *    fate_sys_getgamepath()
 *
 *
 *
 *
 *
 *
 *
 */



#if defined(FATE_DEFS_LINUX)

#if !(BSD_SOURCE || _XOPEN_SOURCE >= 500  \
 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED || _POSIX_C_SOURCE >= 200112L)
#ifdef _GNU_SOURCE
#include <sys/syscall.h>
static inline ssize_t readlink(const char *path, char *buf, size_t bufsiz) {
    return syscall(SYS_readlink, path, buf, bufsiz);
}
static inline int lstat(const char *path, struct stat *buf) {
    return syscall(SYS_lstat, path, buf);
}
#else
#error Syscalls lstat() and readlink() are not available.
#endif
#endif
static char *get_executable_path(void) {
    struct stat st;
    char *str = (char*)getauxval(AT_EXECFN), *str2;
    if(str)
        return realpath(str, NULL);
    if(lstat("/proc/self/exe", &st) == 0) {
        str = malloc(st.st_size+1);
        if(readlink("/proc/self/exe", str, st.st_size) > 0) {
            str[st.st_size] = '\0';
            str2 = realpath(str, NULL);
            free(str);
            return str2;
        }
        free(str);
    }
    return NULL;
}

#elif defined(FATE_DEFS_FREEBSD)

static char *get_executable_path(void) {
    static char appdir[PATH_MAX];
    char procfs[2][19] = {
        "/proc/curproc/exe",
        "/proc/curproc/file"
    };
    int i, mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
    size_t size = PATH_MAX;
    struct stat st;
    char *str, *str2;
    for(i=0 ; i<2 ; ++i) {
        if(lstat(procfs[i], &st) == 0) {
            str = malloc(st.st_size+1);
            if(readlink(procfs[i], str, st.st_size) > 0) {
                str[st.st_size] = '\0';
                str2 = realpath(str, NULL);
                free(str);
                return str2;
            }
            free(str);
        }
    }
    if(sysctl(mib, 4, appdir, &size, NULL, 0) == 0)
        return realpath(appdir, NULL);
    return NULL;
}


#elif defined(FATE_DEFS_OSX)

static char *get_executable_path(void) {
    static char appdir[MAXPATHLEN];
    char *str, *str2;
    size_t size = MAXPATHLEN;
    if(_NSGetExecutablePath(appdir, &size) == 0)
        return realpath(appdir, NULL);
    str = malloc(size);
    _NSGetExecutablePath(str, &size);
    str2 = realpath(str, NULL);
    free(str);
    return str2;
}

#elif defined(FATE_DEFS_WINDOWS)
static char *get_executable_path(void) {
    return strdup(_pgmptr);
}
#endif

#if defined(FATE_DEFS_WINDOWS)
#define PATHSEP "\\"
#else
#define PATHSEP "/"
#endif

static bool remove_last_path_component(char *path) {
    char *ind = strrchr(path, *PATHSEP);
    if(*ind) {
        *ind = '\0';
        return true;
    }
    return false;
}

char *fate_sys_getgamepath(void) {
    char *expath = get_executable_path();
    if(!expath)
        return NULL;
    size_t size = strlen(expath);
    char *res  = malloc(size+strlen(PATHSEP"res")+1);
    char *data = malloc(size+strlen(PATHSEP"data")+1);

    int i;
    for(i=0 ; i<3 ; ++i) {
        if(!remove_last_path_component(expath))
            break;
        strcpy(res,  expath);
        strcat(res,  PATHSEP"res");
        strcpy(data, expath);
        strcat(data, PATHSEP"data");
        if(fate_sys_file_exists(res) && fate_sys_file_exists(data)) {
            free(res);
            free(data);
            return expath;
        }
    }
    free(res);
    free(data);
    fate_logf_err("Could not find res/ and data/ directories.\n");
    return NULL;
}

#undef PATHSEP



/*
 *
 *
 *
 *
 *
 *
 *
 *    fate_sys_log_stacktrace()
 *
 *
 *
 *
 *
 *
 *
 */



#ifdef FATE_DEFS_WINDOWS

/* See http://stackoverflow.com/questions/5693192/win32-backtrace-from-c-code */
void fate_sys_log_stacktrace(void (*logfunc)(const char *fmt, ...))
{
    unsigned i, j;
    unsigned short frames;
    void *stack[FATE_DEFS_STACKTRACE_FRAMES_CAPACITY];
    TCHAR modname[FATE_DEFS_STACKTRACE_MODULE_NAME_CAPACITY];
    DWORD modname_len;
    SYMBOL_INFO *symbol;
    HANDLE process;
    HMODULE modhandle;

    process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    frames = CaptureStackBackTrace(0, FATE_DEFS_STACKTRACE_FRAMES_CAPACITY,
                                   stack, NULL);
    /* Do not change this. The structure needs indeed to be 
     * allocated from the heap. */
    symbol = calloc(sizeof(SYMBOL_INFO)+256, 1);
    if(!symbol) {
        logfunc("log_stacktrace : Could not allocate "
                "SYMBOL_INFO object.\r\n");
        SymCleanup(process);
        return;
    }
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for(i=0 ; i<frames ; ++i) {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
#if _WIN32_WINNT >= 0x0501
        if(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, 
                            (LPCSTR) symbol->ModBase, &modhandle)) 
        {
            modname_len = GetModuleFileName(modhandle, modname, 
                              FATE_DEFS_STACKTRACE_MODULE_NAME_CAPACITY);
            /* MS says that on Windows XP, the string is not null-terminated. */
            for(j=0 ; j<modname_len ; j++)
                logfunc("%c", modname[j]);
        }
#endif
        if(symbol->MaxNameLen > 0)
            logfunc("(%s) ", symbol->Name);
        logfunc("[0x%llx]", symbol->Address);
        if(symbol->Flags & SYMFLAG_VALUEPRESENT)
            logfunc(" (Value : 0x%llx)", symbol->Value);
        logfunc("\r\n");
    }

    free(symbol);
    SymCleanup(process);
}

#else /* !FATE_DEFS_WINDOWS */


void fate_sys_log_stacktrace(void (*logfunc)(const char *fmt, ...)) {
    void *buffer[FATE_DEFS_STACKTRACE_FRAMES_CAPACITY];
    char **strings;
    int i, num_strings;

    num_strings = backtrace(buffer, FATE_DEFS_STACKTRACE_FRAMES_CAPACITY);
    strings = backtrace_symbols(buffer, num_strings);
    if(!strings) {
        logfunc("Could not get a stacktrace.\n");
        return;
    }
    for(i=0; i<num_strings; ++i)
        logfunc("%s\n", strings[i]);
    free(strings);
}

#endif /* FATE_DEFS_WINDOWS */


/*
 *
 *
 *
 *
 *
 *
 *
 *    fate_sys_crash_handler_setup()
 *
 *
 *
 *
 *
 *
 *
 */


#ifdef FATE_DEFS_WINDOWS

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
        fate_sys_log_stacktrace(&fate_logf_err);

    fate_globalstate_deinit(fate_gs);
    exit(EXIT_FAILURE); /* Not abort(). */

    return EXCEPTION_EXECUTE_HANDLER;
}
 
void fate_sys_crash_handler_setup(void) {
    SetUnhandledExceptionFilter(&fate_win32_exception_handler);
}

#else /* Unices */

#if !(_POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE)

/* Very unlikely, but since it is most often compiled out, it causes no harm. */
void fate_sys_crash_handler_setup(void) {
    fate_logf_err("The POSIX signal handler was not available "
                  "at compilation.\n");
}

#else  /* _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#if _POSIX_C_SOURCE >= 199309L
void fate_sys_sigaction(int signum, siginfo_t *siginfo, void *ucontext)
#else
void fate_sys_sighandler(int signum)
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
        fate_sys_log_stacktrace(fate_logf_err);

    fate_globalstate_deinit(fate_gs);

    if(fatal)
         exit(EXIT_FAILURE); /* Not abort(). It will send a signal too. */
}

void fate_sys_crash_handler_setup(void) {
    struct sigaction sa;

    memset(&sa, 0, sizeof(struct sigaction));

    sa.sa_flags = SA_RESTART;
#if _POSIX_C_SOURCE >= 199309L
    sa.sa_flags |= SA_SIGINFO;
    sa.sa_sigaction = &fate_sys_sigaction;
#else
    sa.sa_handler = &fate_sys_sighandler;
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



/* Test suite.

unsigned recursive(unsigned d) {
    if(d==1) {
        fate_logf_err("--- Early stack trace ---\n");
        fate_sys_log_stacktrace(&fate_logf_err);
        fate_logf_err("--- Stack trace ---\n");
    }
    unsigned foo = 100/(d--);
    return recursive(d);
}

int main(void) {
    fate_sys_crash_handler_setup();
    fate_logf_err("%u\n", recursive(4));
    return 0;
}
*/

