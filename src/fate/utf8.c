#include <fate/utf8.h>
#include <fate/mem.h>

#ifdef FE_TARGET_WINDOWS
#include <windows.h>
WCHAR *fe_utf8_to_win32unicode(const char *utf8) {
    /* char_count includes the NULL terminator. */
    int wcount = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    WCHAR *wstr = fe_mem_heapalloc(char_count, WCHAR, "");
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, wcount);
    return wstr;
}
#endif
