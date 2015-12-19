
#define FATE_I18N

#ifdef FATE_I18N
#include <libintl.h>
#include <locale.h>
#endif
#include <stdio.h>

// See http://fedoraproject.org/wiki/How_to_do_I18N_through_gettext

#ifdef FATE_I18N
#define _(X) gettext(X)
#define P_(X,Y,N) ngettext(X,Y,N)
#else
#define _(X) (X)
#define P_(X,Y,N) (N==1 ? X : Y)
#endif

/* See https://www.gnu.org/software/gettext/manual/html_node/gettext-grok.html */
void fate_i18n_language(const char *lang) {
#ifdef FATE_I18N
    setenv("LANGUAGE", lang, 1);
    {
        extern int _nl_msg_cat_cntr;
        ++_nl_msg_cat_cntr;
    }
#endif
}

void fate_i18n_setup(void) {
#ifdef FATE_I18N
    setlocale(LC_ALL, "");
    bindtextdomain("fate", "res/locale");
    bind_textdomain_codeset("fate", "UTF-8");
    textdomain("fate");
#endif
}

int main(int argc, char *argv[]) {
    fate_i18n_setup();
    fate_i18n_language("en");
    printf(_("Hello world!\n"));
    fate_i18n_language("fr");
    printf(_("Hello world!\n"));
    return 0;
}

