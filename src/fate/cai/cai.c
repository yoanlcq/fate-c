#include <fate/log.h>

static const char *TAG = "fe_cai";

#if defined(_MSC_VER)
    #include <windows.h>
    /* TODO: 
     * - Always have DBGHELP_TRANSLATE_TCHAR defined.
     * - Use SymFromAddrW() to translate fn_vaddr to name.
     */
    //static fe_mt_spinlock dbghelp_spinlock;
    void fe_cai_penter(void *fn_vaddr) {
        //fe_mt_spinlock_lock(&dbghelp_spinlock);
        //SymFromAddrW(...);
        //fe_mt_spinlock_unlock(&dbghelp_spinlock);
        //fe_logi(TAG, "Entering %p\n", fn_vaddr);
        #define PENTER_MSG "fe_cai: Entering <somefunc>.\n"
        DWORD written;
        /*
        WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), 
                TEXT(PENTER_MSG), sizeof(PENTER_MSG), &written, NULL);
                */
    }
    void fe_cai_pexit(void *fn_vaddr) {
        //fe_logi(TAG, "Leaving  %p\n", fn_vaddr);
        #define PEXIT_MSG "fe_cai:  Exiting  <somefunc>.\n"
        DWORD written;
        /*
        WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), 
                TEXT(PEXIT_MSG), sizeof(PEXIT_MSG), &written, NULL);
                */
    }
    #ifdef _M_IX86
        #define MS_HOOK(action) \
            void __declspec(naked) __cdecl _p##action(void) { \
                __asm pushad \
                __asm mov  eax, esp \
                __asm add  eax, 32 \
                __asm mov  eax, dword ptr[eax] \
                __asm sub  eax, 5 \
                __asm push eax \
                __asm call fe_cai_p##action \
                __asm pop eax \
                __asm popad \
                __asm ret \
            }
        MS_HOOK(enter)
        MS_HOOK(exit)
        #undef MS_HOOK
    #endif
#elif defined(__GNUC__)
    __attribute__((no_instrument_function))
    void fe_cai_func_enter(void *this_fn, void *call_site) {
        /*fe_logi(TAG, "Entering %p at %p\n", this_fn, call_site);*/
    }
    __attribute__((no_instrument_function))
    void fe_cai_func_exit(void *this_fn, void *call_site) {
        /*fe_logi(TAG, "Leaving  %p at %p\n", this_fn, call_site);*/
    }
    __attribute__((no_instrument_function))
    void __cyg_profile_func_enter (void *this_fn, void *call_site) {
        fe_cai_func_enter(this_fn, call_site);
    }
    __attribute__((no_instrument_function))
    void __cyg_profile_func_exit  (void *this_fn, void *call_site) {
        fe_cai_func_exit(this_fn, call_site);
    }
#endif

