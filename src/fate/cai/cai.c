#include <fate/fate.h>

#define TAG "fe_cai"

#if defined(_MSC_VER)
    void fe_cai_penter(void *fn_vaddr) {
        #define PENTER_MSG "fe_cai: Entering <somefunc>.\n"
        /*
        DWORD written;
        WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), 
                TEXT(PENTER_MSG), sizeof(PENTER_MSG), &written, NULL);
                */
    }
    void fe_cai_pexit(void *fn_vaddr) {
        #define PEXIT_MSG "fe_cai:  Exiting  <somefunc>.\n"
        /*
        DWORD written;
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
    FE_DECL_NO_CAI
    void __cyg_profile_func_enter (void *this_fn, void *call_site) {
        fe_dbg_sym sym, callsym;
        if(!fe_dbg_sym_init(&sym, this_fn))
            fe_logi(TAG, "Failed...\n");
        if(!fe_dbg_sym_init(&callsym, call_site))
            fe_logi(TAG, "Failed...\n");
        fe_logi(TAG, "Entering [%p](%s) from [%p](%s)\n", 
                this_fn, sym.name, call_site, callsym.name);
        fe_dbg_sym_deinit(&sym);
        fe_dbg_sym_deinit(&callsym);
    }
    FE_DECL_NO_CAI
    void __cyg_profile_func_exit  (void *this_fn, void *call_site) {
        fe_logi(TAG, "Leaving  [%p] at [%p]\n", this_fn, call_site);
    }
#endif

