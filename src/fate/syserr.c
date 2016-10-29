#include <fate/fate.h>

#ifdef FE_TARGET_WINDOWS
static char* syserr_str(fe_syserr error) {
    LPWSTR lpMsgBuf;

    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        /*MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),*/
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (void*)&lpMsgBuf,
        0, NULL);

    char *utf8 = fe_utf8_from_win32unicode(lpMsgBuf);
    LocalFree(lpMsgBuf);
    return utf8;
}
#elif _XOPEN_SOURCE >= 700
#include <locale.h>
static char* syserr_str(fe_syserr err) {
    /* Replace "C" by "" to generate a locale suitable for the user. 
     * XXX We should allocate it only once. */
    locale_t lc = newlocale(LC_ALL_MASK, "C", 0);
    char *str = fe_asprintf("%s", strerror_l(err, lc));
    freelocale(lc);
    return str;
}
#else
static char* syserr_str(fe_syserr err) {
    return fe_asprintf("%s", strerror(err));
}
#endif

char *fe_syserr_str(fe_syserr err) {
    if(fe_syserr_is_user_defined(err))
        return fe_strdup("User-defined error."); /* XXX */
    return syserr_str(err);
}
