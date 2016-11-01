#include <fate/fate.h>

#define TAG "fe_dbg"

#ifdef FE_TARGET_WINDOWS

static HANDLE process = NULL;
static fe_mt_spinlock dh_lock;


static void windows_api_logfail(char *funcname, DWORD err) {
    char *str = fe_syserr_str(err);
    fe_logd(TAG, "%s() failed with error %d: %s\n", funcname, err, str);
    fe_mem_heapfree(str);
}

/* String allocation party ! */
void fe_dbg_sym_setup(void) {
    process = GetCurrentProcess();
    fe_mt_spinlock_lock(&dh_lock);
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS
                | SYMOPT_LOAD_LINES);    
    fe_fpath exe_fpath = fe_fpath_windows_executable_folder("");
    char *spath = fe_asprintf("%s;%s\\..;%s\\..\\..", 
        exe_fpath.path, exe_fpath.path, exe_fpath.path);
    fe_logd(TAG, "Using `%s' as the search path for SymInitializeW().\n",
            spath);
    WCHAR *spath_w = fe_utf8_to_win32unicode(spath);
    bool success = SymInitializeW(process, spath_w, TRUE);
    fe_mt_spinlock_unlock(&dh_lock);
    if(!success)
        windows_api_logfail("SymInitializeW", GetLastError());
    fe_mem_heapfree(spath_w);
    fe_mem_heapfree(spath);
    fe_fpath_deinit(exe_fpath);
}
void fe_dbg_sym_cleanup(void) {
    fe_mt_spinlock_lock(&dh_lock);
    SymCleanup(process);
    fe_mt_spinlock_unlock(&dh_lock);
}


bool fe_dbg_sym_init(fe_dbg_sym *sym, void *addr) {
    memset(sym, 0, sizeof *sym);

#define max_modname_len 256
    WCHAR modname[max_modname_len];
    DWORD modname_len;
    HMODULE modhandle;

    const size_t max_name_len = 255;
    /* Do not change this. The structure needs indeed to be 
     * allocated from the heap. */
    SYMBOL_INFOW *symbol 
        = calloc(sizeof(SYMBOL_INFOW) + sizeof(WCHAR)*(max_name_len+1), 1);
    if(!symbol) {
        fe_logd(TAG, "Failed to calloc() a SYMBOL_INFOW structure!\n");
        return false;
    }
    symbol->MaxNameLen = max_name_len;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFOW);

    DWORD64 addr64 = (DWORD64)(uintptr_t)addr;
    fe_mt_spinlock_lock(&dh_lock);
    BOOL success = SymFromAddrW(process, addr64, NULL, symbol);
    fe_mt_spinlock_unlock(&dh_lock);
    if(!success) {
        windows_api_logfail("SymFromAddrW", GetLastError());
        return false;
    }
#if _WIN32_WINNT >= 0x0501
    if(symbol->ModBase) {
        if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, 
                             (LPWSTR)(uintptr_t)(symbol->ModBase), 
                             &modhandle)) 
        {
            modname_len = GetModuleFileNameW(modhandle, modname, 
                              max_modname_len);
            /* MS says that on Windows XP, the string is 
             * not null-terminated. */
            modname[modname_len] = '\0';
            sym->dll_path = fe_utf8_from_win32unicode(modname);
        } else
            windows_api_logfail("GetModuleHandleExW", GetLastError());
    }
#endif

    DWORD col;
    IMAGEHLP_LINEW64 line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    fe_mt_spinlock_lock(&dh_lock);
    if(SymGetLineFromAddr64(process, addr64, &col, &line)) {
        sym->line_number   = line.LineNumber;
        sym->source_file   = fe_utf8_from_win32unicode(line.FileName);
        sym->column_number = col;
    } else windows_api_logfail("SymGetLineFromAddr64", GetLastError());
    fe_mt_spinlock_unlock(&dh_lock);

    sym->addr = symbol->Address;
    if(symbol->MaxNameLen > 0)
        sym->name = fe_utf8_from_win32unicode(symbol->Name);
    if(symbol->Flags & SYMFLAG_VALUEPRESENT) {
        sym->has_value = true;
        sym->value = symbol->Value;
    }

    free(symbol);
    return true;
}
void fe_dbg_sym_deinit(fe_dbg_sym *sym) {
    if(sym->dll_path) 
        fe_mem_heapfree(sym->dll_path);
    if(sym->name)
        fe_mem_heapfree(sym->name);
    if(sym->source_file)
        fe_mem_heapfree(sym->source_file);
}




#elif defined(FE_TARGET_EMSCRIPTEN) /* ^ FE_TARGET_WINDOWS */



void fe_dbg_sym_setup(void) {}
void fe_dbg_sym_cleanup(void) {}
bool fe_dbg_sym_init(fe_dbg_sym *sym, void *addr) { 
    fe_logd(TAG, "%s() is not implemented yet on Emscripten.", __func__);
    return false; 
}
void fe_dbg_sym_deinit(fe_dbg_sym *sym) {}



#else /* ^ FE_TARGET_EMSCRIPTEN */



void fe_dbg_sym_setup(void) {}
void fe_dbg_sym_cleanup(void) {}

bool fe_dbg_sym_init(fe_dbg_sym *sym, void *addr) {
#ifdef FE_TARGET_ANDROID
    fe_logd(TAG, "%s() is not implemented yet on Android.", __func__);
    return false;
#else
    *sym = (fe_dbg_sym){0};
    #if defined(_GNU_SOURCE) && !defined(FE_TARGET_OSX)
        Dl_info info;
        /* dlerror() won't give messages, according to man :( */
        if(!dladdr(addr, &info))
            return false; 
        /* Using fe_strdup() because of "discards const" warning. */
        sym->dll_path = info.dli_fname ? fe_strdup(info.dli_fname) : NULL;
        sym->name     = info.dli_sname ? fe_strdup(info.dli_sname) : NULL;
        sym->dll_addr = info.dli_fbase;
        sym->addr     = info.dli_saddr;
        #if 0
        if(sym->name) {
            /* fe_logi(TAG, "not backtrace...\n"); */
            return true;
        }
        #endif
    #endif
        /* fe_logi(TAG, "backtrace...\n"); */
        char **strings = backtrace_symbols(&addr, 1);
        if(!strings)
            return false;
        sym->name = fe_strdup(*strings);
        free(strings);
        return true;
#endif
}

void fe_dbg_sym_deinit(fe_dbg_sym *sym) {
#ifdef _GNU_SOURCE
    if(sym->dll_path)
        fe_mem_heapfree(sym->dll_path);
#endif
    if(sym->name)
        fe_mem_heapfree(sym->name);
}



#endif
