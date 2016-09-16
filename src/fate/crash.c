/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */

/*! \file fate/sys.c
 *  \brief System-specific functionality and crash handling.
 *
 * TODO
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <fate/defs.h>
#include <fate/globalstate.h>
#include <fate/utf8.h>
#include <fate/mem.h>
#include <fate/crash.h>
#include <fate/log.h>

#ifdef FE_TARGET_EMSCRIPTEN
#include <emscripten.h>
#endif

#if defined(FE_TARGET_WINDOWS)
#include <Windows.h>
#include <DbgHelp.h>
#elif defined(FE_TARGET_LINUX)
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#ifndef FE_TARGET_ANDROID
#include <execinfo.h>
#endif
#elif defined(FE_TARGET_FREEBSD)
#include <limits.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#elif defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS)
#include <stdint.h>
#include <limits.h>
#include <signal.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <execinfo.h>
#endif

static const char *TAG = "fe_crash";


/*
 *
 *
 *
 *
 *
 *
 *
 *    fe_crash_log_stacktrace()
 *
 *
 *
 *
 *
 *
 *
 */


#ifdef FE_TARGET_EMSCRIPTEN
void fe_crash_log_stacktrace(fe_logfunc logfunc) {
    int flags = EM_LOG_C_STACK | EM_LOG_JS_STACK | EM_LOG_FUNC_PARAMS;
    int  size = emscripten_get_callstack(flags, NULL, 0);
    char *buf = fe_mem_heapalloc(size, char, "");
    emscripten_get_callstack(flags, buf, size);
    logfunc(TAG, buf);
    fe_mem_heapfree(buf);
}
#elif defined(FE_TARGET_WINDOWS)

void fe_crash_log_win32_error(fe_logfunc logfunc, 
                              const char *funcstr, DWORD error) 
{
    LPWSTR lpMsgBuf;

    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (void*)&lpMsgBuf,
        0, NULL);

    char *utf8 = fe_utf8_from_win32unicode(lpMsgBuf);
    LocalFree(lpMsgBuf);
    logfunc(TAG, "%s failed with error %lu : %s", funcstr, error, utf8);
    fe_mem_heapfree(utf8);
}

/* See http://stackoverflow.com/questions/5693192/win32-backtrace-from-c-code */
static void fe_crash_log_stacktrace_win32(
                                   fe_logfunc logfunc, 
                                   const DWORD64 *stack, 
                                   unsigned short nframes)
{
    unsigned i, j;
    WCHAR modname[FE_CRASH_MODNAME_LEN];
    DWORD modname_len;
    SYMBOL_INFO *symbol;
    HANDLE process;
    HMODULE modhandle;

    process = GetCurrentProcess();
    if(!SymInitialize(process, NULL, TRUE)) {
        fe_crash_log_win32_error(logfunc, "SymInitialize", GetLastError());
        return;
    }

    /* Do not change this. The structure needs indeed to be 
     * allocated from the heap. */
    symbol = calloc(sizeof(SYMBOL_INFO)+256, 1);
    if(!symbol) {
        logfunc(TAG, "log_stacktrace : Could not allocate SYMBOL_INFO object.\r\n");
        SymCleanup(process);
        return;
    }
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for(i=0 ; i<nframes ; ++i) {
        if(!SymFromAddr(process, stack[i], 0, symbol)) {
            fe_crash_log_win32_error(logfunc, "SymFromAddr", GetLastError());
            continue;
        }
#if _WIN32_WINNT >= 0x0501
        if(symbol->ModBase) {
            if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, 
                                 (LPWSTR)(uintptr_t)(symbol->ModBase), 
                                 &modhandle)) 
            {
                modname_len = GetModuleFileNameW(modhandle, modname, 
                                  FE_CRASH_MODNAME_LEN);
                /* MS says that on Windows XP, the string is 
                 * not null-terminated. */
                modname[modname_len] = '\0';
                char *modname_utf8 = fe_utf8_from_win32unicode(modname);
                logfunc(TAG, "%s", modname_utf8);
                fe_mem_heapfree(modname_utf8);
            } else {
                fe_crash_log_win32_error(logfunc, "GetModuleHandleExW",
                                         GetLastError());
            }
        }
#endif

        DWORD col;
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        if(SymGetLineFromAddr64(process, stack[i], &col, &line))
            logfunc(TAG, "(%s:%lu:%lu) ", line.FileName, line.LineNumber, col);
        else {
            DWORD err = GetLastError();
            if(err != 487)
                fe_crash_log_win32_error(logfunc, "SymGetLineFromAddr64", err);
        }

        logfunc(TAG, "[0x%"PRIx64"] ", symbol->Address);
        if(symbol->MaxNameLen > 0)
            logfunc(TAG, "(%s)", symbol->Name);
        if(symbol->Flags & SYMFLAG_VALUEPRESENT)
            logfunc(TAG, "(Value : 0x%"PRIx64")", symbol->Value);
        logfunc(TAG, "\r\n");
    }

    free(symbol);
    SymCleanup(process);
}
void fe_crash_log_stacktrace(fe_logfunc logfunc)
{
    PVOID stack[FE_CRASH_STACK_LEN];
    DWORD64 stack_dw[FE_CRASH_STACK_LEN];

    unsigned short i, nframes;
    nframes = CaptureStackBackTrace(0, FE_CRASH_STACK_LEN,
                                    stack, NULL);
    for(i=0 ; i<nframes ; ++i)
        stack_dw[i] = (DWORD64)(uintptr_t)stack[i];
    fe_crash_log_stacktrace_win32(logfunc, stack_dw, nframes);
}

#else /* !FE_TARGET_WINDOWS */


void fe_crash_log_stacktrace(fe_logfunc logfunc) {
#ifdef FE_TARGET_ANDROID
    logfunc(TAG, "TODO : Support stacktraces on Android.\n");
#else
    void *buffer[FE_CRASH_STACK_LEN];
    char **strings;
    int i, num_strings;

    num_strings = backtrace(buffer, FE_CRASH_STACK_LEN);
    strings = backtrace_symbols(buffer, num_strings);
    if(!strings) {
        logfunc(TAG, "Could not get a stacktrace.\n");
        return;
    }
    for(i=0; i<num_strings; ++i)
        logfunc(TAG, "%s\n", strings[i]);
    free(strings);
#endif
}

#endif /* FE_TARGET_WINDOWS */


/*
 *
 *
 *
 *
 *
 *
 *
 *    fe_crash_setup()
 *
 *
 *
 *
 *
 *
 *
 */

#ifdef FE_TARGET_EMSCRIPTEN
void fe_crash_setup(void) {
    /* FIXME */
}
#elif defined(FE_TARGET_WINDOWS)

/* See http://spin.atomicobject.com/2013/01/13/exceptions-stack-traces-c/ */
LONG CALLBACK fe_crash_win32_exception_handler(EXCEPTION_POINTERS *ep)
{
    fe_loge(TAG, "Process received ");
#define HELPER(_S) \
    case _S: fe_loge(TAG, #_S); break
#define DEFAULT \
    default: fe_loge(TAG, "Unrecognized exception"); break
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
    fe_loge(TAG, " (%scontinuable, at %p).\r\n", 
                  ep->ExceptionRecord->ExceptionFlags ? "non" : "",
                  ep->ExceptionRecord->ExceptionAddress
                  );

    if(ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION
    || ep->ExceptionRecord->ExceptionCode == EXCEPTION_IN_PAGE_ERROR) {
        fe_loge(TAG, "(Thread ");
        if(ep->ExceptionRecord->ExceptionInformation[0] == 8)
            fe_loge(TAG, "caused a user-mode data execution "
                          "prevention violation ");
        else
            fe_loge(TAG, "attempted to %s inacessible data ",
                          ep->ExceptionRecord->ExceptionInformation[0]
                              ? "write to" : "read");
        fe_loge(TAG, "at %p", 
                      (void*)ep->ExceptionRecord->ExceptionInformation[1]);
        if(ep->ExceptionRecord->ExceptionCode == EXCEPTION_IN_PAGE_ERROR)
            fe_loge(TAG, ", NTSTATUS code is %p", 
                          (void*)ep->ExceptionRecord->ExceptionInformation[2]);
        fe_loge(TAG, ")\r\n");
    }

    if(ep->ExceptionRecord->ExceptionCode != EXCEPTION_STACK_OVERFLOW) {

        DWORD64 stack[FE_CRASH_STACK_LEN];
        unsigned short nframes;
        // StackWalk64() may modify context record passed to it, so we will
        // use a copy.
        CONTEXT context_record = *(ep->ContextRecord);
        // Initialize stack walking.
        STACKFRAME64 stack_frame;
        memset(&stack_frame, 0, sizeof(stack_frame));
#if defined(_WIN64)
        int machine_type = IMAGE_FILE_MACHINE_AMD64;
        stack_frame.AddrPC.Offset = context_record.Rip;
        stack_frame.AddrFrame.Offset = context_record.Rbp;
        stack_frame.AddrStack.Offset = context_record.Rsp;
#else
        int machine_type = IMAGE_FILE_MACHINE_I386;
        stack_frame.AddrPC.Offset = context_record.Eip;
        stack_frame.AddrFrame.Offset = context_record.Ebp;
        stack_frame.AddrStack.Offset = context_record.Esp;
#endif
        stack_frame.AddrPC.Mode = AddrModeFlat;
        stack_frame.AddrFrame.Mode = AddrModeFlat;
        stack_frame.AddrStack.Mode = AddrModeFlat;
        nframes = 0;

        while (StackWalk64(machine_type,
                           GetCurrentProcess(),
                           GetCurrentThread(),
                           &stack_frame,
                           &context_record,
                           NULL,
                           &SymFunctionTableAccess64,
                           &SymGetModuleBase64,
                           NULL) 
                && nframes < FE_CRASH_STACK_LEN) {
            stack[nframes++] = stack_frame.AddrPC.Offset;
        }

        fe_crash_log_stacktrace_win32(fe_loge, stack, nframes);
    }

    fe_globalstate_deinit(fe_gs);
    exit(EXIT_FAILURE); /* Not abort(). */

    return EXCEPTION_CONTINUE_SEARCH;
}
 
void fe_crash_setup(void) {
    /* SetUnhandledExceptionFilter(fe_crash_win32_exception_handler); */
    AddVectoredExceptionHandler(1, fe_crash_win32_exception_handler);
}

#else /* Unices */

#if !(_POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE)

/* Very unlikely, but since it is most often compiled out, it causes no harm. */
void fe_crash_setup(void) {
    fe_loge(TAG, "The POSIX signal handler was not available "
                  "at compilation.\n");
}

#else  /* _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#if _POSIX_C_SOURCE >= 199309L
void fe_crash_sigaction(int signum, siginfo_t *siginfo, void *ucontext)
#else
void fe_crash_sighandler(int signum)
#endif
{
    int btrace, fatal;

    fe_loge(TAG, "Process received ");

#define HELPER(S_, A_, B_, C_) \
    case S_: fe_loge(TAG, #S_ ": " A_); B_; C_; break
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
    default: fe_loge(TAG, "signal #%d", signum);    btrace=1, fatal=0; break;
    }
#undef HELPER

#if _POSIX_C_SOURCE >= 199309L

#define HELPER(S_, A_, B_, C_) \
    case S_: fe_loge(TAG, " (%s%s%s)", A_, B_, C_); break
#define DEFAULT \
    default: fe_loge(TAG, " (siginfo->si_code = %d)", siginfo->si_code); break

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

    fe_loge(TAG, ".\n");
    if(btrace)
        fe_crash_log_stacktrace(fe_loge);

    fe_globalstate_deinit(fe_gs);

    if(fatal)
         exit(EXIT_FAILURE); /* Not abort(). It will send a signal too. */
}

void fe_crash_setup(void) {
    struct sigaction sa;

    memset(&sa, 0, sizeof(struct sigaction));

    sa.sa_flags = SA_RESTART;
#if _POSIX_C_SOURCE >= 199309L
    sa.sa_flags |= SA_SIGINFO;
    sa.sa_sigaction = &fe_crash_sigaction;
#else
    sa.sa_handler = &fe_crash_sighandler;
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
        fe_loge(TAG, "--- Early stack trace ---\n");
        fe_crash_log_stacktrace(&fe_loge);
        fe_loge(TAG, "--- Stack trace ---\n");
    }
    unsigned foo = 100/(d--);
    return recursive(d);
}

int main(void) {
    fe_crash_setup();
    fe_loge(TAG, "%u\n", recursive(4));
    return 0;
}
*/

