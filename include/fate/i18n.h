/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */

/*! \file fate/i18n.h
 *  \brief Internationalization.
 *  \defgroup i18n i18n : Internationalization.
 *
 *
 * <b><i>Newcomers are strongly encouraged to first read 
 * <a href="http://fedoraproject.org/wiki/How_to_do_I18N_through_gettext" 
 * target="_blank">this article from the Fedora project wiki</a>.</i></b>
 *
 * F.A.T.E's default language is English, for obvious reasons, but support
 * for other languages is always considered.
 *
 * This module provides macros to translate the <i>engine</i>'s strings, 
 * which are, by definition, primarily intended for game developers.<br>
 *
 * The language is neither fixed at compile-time nor at runtime, and can 
 * be changed at any point during the program's execution.
 *
 * Localization (L10N) is performed through .mo files, located under
 * <tt>res/locale/<lang>/LC_MESSAGES/fate.mo</tt>.<br>
 * <b>TODO</b> : Explain clearly the localization process.
 *
 * The game developers may either use this module or provide their own way of 
 * internationalizing their product (such as using appropriate JSON files for 
 * in-game dialogs, etc). Both approaches are acceptable as each has its
 * share of pros and cons.
 *
 * Selfish developers may choose to define \c FE_NO_I18N if \c gettext() is
 * too much overhead for them, or they don't feel like having all those
 * '.mo' files and stuff to manage.<br>
 * As about anything in F.A.T.E, support for selfish developers is always 
 * considered.
 *
 * <b>Compile-time switches :</b>
 * <table>
 * <tr><td>FE_NO_I18N</td>
 * <td>If defined, this module's functionalities are compiled out.</td>
 * </tr>
 * </table>
 *
 * <b>Module dependencies :</b>
 * - fe_log
 *
 * Example : Print "Hello world!", then its french translation.<br>
 * Requires that the .mo file under \c res/locale/fr/LC_MESSAGES/fate.mo is 
 * translated appropriately.
 * \code{.c}
 * #include <fate/log.h>
 * #include <fate/i18n.h>
 *
 * static const char *TAG="i18n example";
 *
 * int main(int argc, char *argv[]) {
 *     fe_log_setup();
 *     fe_i18n_setup();
 *     fe_i18n_switchlang("en");
 *     fe_logi(TAG, _("Hello world!\n"));
 *     fe_i18n_switchlang("fr");
 *     fe_logi(TAG, _("Hello world!\n"));
 *     return 0;
 * }
 * \endcode
 *
 * @{
 */

#ifndef FE_I18N_H
#define FE_I18N_H

//#include <libintl.h>
#include <locale.h>

#ifndef FE_NO_I18N
/*! \brief Helper macro expanding to \c gettext(), or its own argument if
 *         FE_NO_I18N is defined. */
#define _(X) gettext(X)
/*! \brief Helper macro expanding to \c ngettext(), or a simple ternary
 *         operator call if FE_NO_I18N is defined. */
#define PLURAL(X,Y,N) ngettext(X,Y,N)
#else
#define _(X) (X)
#define PLURAL(X,Y,N) (N==1 ? X : Y)
#endif

/*! \brief Call this once to setup the i18n module. 
 *
 * It does <i>not</i> call #fe_i18n_switchlang().
 * The caller is still invited to at least call 
 * #fe_i18n_switchlang("en") right after.
 *
 * Calling it more than once should cause no harm but it is discouraged.
 */
void fe_i18n_setup(void);

/*! \brief Changes the language, affecting calls to \c gettext() and 
 *         \c ngettext().
 *
 * \param lang A two-character string identifying a language, such as "en" 
 *        or "fr".<br>
 *        See also 
 * <a href="http://www.loc.gov/standards/iso639-2/php/code_list.php" 
 * target="_blank">the ISO 639-1 codes list</a> (2nd column).
 */
void fe_i18n_switchlang(const char *lang);

/*! @} */

#endif /* FE_I18N_H */
