#include <fate/defs.h>

#ifdef FATE_DEFS_WINDOWS

#include <stdlib.h>
#include <Windows.h>
#include <DbgHelp.h>
#include <fate/log.h>

/* See http://stackoverflow.com/questions/5693192/win32-backtrace-from-c-code */
void fate_log_stacktrace(void (*logfunc)(const char *fmt, ...))
{
    unsigned i;
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
                             symbol->ModBase, &modhandle) 
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

#include <stdlib.h>
#include <execinfo.h>
#include <fate/log.h>

void fate_log_stacktrace(void (*logfunc)(const char *fmt, ...)) {
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

