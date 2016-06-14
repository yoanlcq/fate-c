#include <stdlib.h>
#include <fate/defs.h>
#include <fate/i18n.h>
#include <fate/log.h>

#ifdef FE_LOG_USE_VERBOSE
static const char *TAG="fe_i18n";
#endif

/* See https://www.gnu.org/software/gettext/manual/html_node/gettext-grok.html */
void fe_i18n_switchlang(const char *lang) {
    fe_logv(TAG, _("Now switching language to \"%s\".\n"), lang);
    setenv("LANGUAGE", lang, 1);
#ifdef FE_TARGET_LINUX 
    {
        /* This is for GNU gettext() only. */
        extern int _nl_msg_cat_cntr;
        ++_nl_msg_cat_cntr;
    }
#endif
}

void fe_i18n_setup(void) {
    const char *td = "res/locale";
    setlocale(LC_ALL, "");
    bindtextdomain("fate", td);
    bind_textdomain_codeset("fate", "UTF-8");
    textdomain("fate");
    fe_logv(TAG, 
        _("I18N has been set up. Text domain bound under \"%s\".\n"), td);
}
