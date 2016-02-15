#include <fate/i18n.h>
#include <fate/log.h>

static const char *TAG="i18n";

/* See https://www.gnu.org/software/gettext/manual/html_node/gettext-grok.html */
void fate_i18n_switchlang(const char *lang) {
    fate_logv(TAG, _("Now switching language to \"%s\".\n"), lang);
    setenv("LANGUAGE", lang, 1);
    {
        extern int _nl_msg_cat_cntr;
        ++_nl_msg_cat_cntr;
    }
}

void fate_i18n_setup(void) {
    const char *td = "res/locale";
    setlocale(LC_ALL, "");
    bindtextdomain("fate", td);
    bind_textdomain_codeset("fate", "UTF-8");
    textdomain("fate");
    fate_logv(TAG, 
        _("I18N has been set up. Text domain bound under \"%s\".\n"), td);
}
