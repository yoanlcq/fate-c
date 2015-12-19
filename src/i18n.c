#include <libintl.h>
#include <locale.h>
#include <stdio.h>

// See http://fedoraproject.org/wiki/How_to_do_I18N_through_gettext

#ifdef FATE_I18N
#define _(X) gettext(X)
#else
#define _(X) (X)
#endif

/* See https://www.gnu.org/software/gettext/manual/html_node/gettext-grok.html */
void switch_language(const char *lang) {
    setenv("LANGUAGE", lang, 1);
    {
        extern int _nl_msg_cat_cntr;
        ++_nl_msg_cat_cntr;
    }
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    bindtextdomain("helloworld", "res/locale");
    textdomain("helloworld");
    printf(_("Hello world!\n"));
    return 0;
}

/*
 * mkdir po/
 * xgettext -d helloworld -o po/helloworld.pot -k_ -s helloworld.c
 * mkdir <lang>/
 * cp helloworld.pot <lang>/helloworld.po
 * # Modifier le .po...
 * # Compilation :
 * msgfmt helloworld.po -o helloworld.mo
 * cp helloworld.mo res/locale/<lang>/LC_MESSAGES/
 * LANG=<lang>
 * ./helloworld
 */
