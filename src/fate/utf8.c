#include <fate/utf8.h>
#include <fate/mem.h>

#ifdef FE_TARGET_WINDOWS
#include <windows.h>
WCHAR *fe_utf8_to_win32unicode(const char *utf8) {
    /* char_count includes the NULL terminator. */
    int wcount = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    WCHAR *wstr = fe_mem_heapalloc(wcount, WCHAR, "");
    if(!wstr)
        return NULL;
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, wcount);
    return wstr;
}
char *fe_utf8_from_win32unicode(const WCHAR *unicode) {
    int utf8count = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
    char *utf8 = fe_mem_heapalloc(utf8count, char, "");
    if(!utf8)
        return NULL;
    WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, utf8count, NULL, NULL);
    return utf8;
}
#endif
