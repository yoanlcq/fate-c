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

/*! \file fate/log.h
 *  \brief The logging module, multiplexing output.
 *  \defgroup log Log
 *
 * This module is, within the engine, the standard way of logging messages 
 * with various levels of severity.
 * It addresses the following needs :
 * - Abstraction for platform-specific code (notably Android and
 *   Emscripten);
 * - Multiplex to several outputs (console and files), which can 
 *   be added/removed at startup or at runtime;
 * - Disable outputs based on their tag or their severity;
 * - Automatically display more info, such as the date, thread ID, 
 *   or others.
 *
 * This module used to provide a log function for each external module, such
 * as video, audio, etc. but that proved not to be scalable.<br>
 * The current version is inspired from Android's Log module and is far more
 * flexible.<br>
 * The log functions actually translate to calls like fprintf() or 
 * platform-specific alternatives.
 * #fe_logc may use \c SDL_ShowSimpleMessageBox() or a Javascript \c alert().
 * 
 * Each other module is encouraged to declare a private string which they
 * should pass as the log functions' first parameter, like so :
 * \code{.c}
 * static const char *TAG = "Module name";
 * ...
 * fe_logi(TAG, _("The answer is %u\n"), 42);
 * \endcode
 *
 * In each one of the log functions, the "tag" parameter may never be NULL.
 * You may however use an empty string instead.
 *
 * @{
 */

#ifndef FE_LOG_H
#define FE_LOG_H

#include <fate/defs.h>
#include <stdarg.h>

/*! \brief Type for the log functions.
 *
 * This is intended for use by other modules, which would like to expose
 * functions taking a log function as a parameter.
 */
typedef void (*fe_logfunc)(const char *tag, const char *fmt, ...) FE_PRINTF_DECL(2,3);

/*! \brief Setup this module.
 * 
 * This function should be called once to initialize this module.
 * Calling it more than once causes no harm but it is discouraged.
 *
 * This function also calls #fe_log_flags, with #FE_LOG_COLORED and #FE_LOG_TAG.
 */
void fe_log_setup(void);

/*! \brief Clean-up this module.
 * 
 */
void fe_log_cleanup(void);


/*! \brief Flags to be given to #fe_log_flags.
 *
 */
enum fe_log_flag {
    FE_LOG_COLORED  = 1,    /*!< When logging to consoles, attempt to 
        color the output based on the severity (enabled by default). */
    FE_LOG_DATE     = 1<<1, /*!< Display the date, as defined by the locale.*/
    FE_LOG_TIME     = 1<<2, /*!< Display the time since the log module was
        set up. */
    FE_LOG_PID      = 1<<3, /*!< Display the app's processus ID. */
    FE_LOG_TID      = 1<<4, /*!< Display the current thread ID. */
    FE_LOG_TICKNO   = 1<<5, /*!< Display the current tick number. */
    FE_LOG_FRAMENO  = 1<<6, /*!< Display the current frame number.  */
    FE_LOG_TAG      = 1<<7, /*!< Display the tag (enabled by default).*/
    FE_LOG_SEVERITY = 1<<8  /*!< Display the severity. Should not be needed 
        if #FE_LOG_COLORED is raised. */
};
typedef enum fe_log_flag fe_log_flag;

/*! \brief Atomically set the log module's flags, mostly to display 
 * extra columns.
 *
 * Please acknowledge that, depending on the platform (and other conditions),
 * some flags may lose their effect.
 *
 * On Android, logs are managed by Logcat, and F.A.T.E cannot (as of today)
 * interact with it. However the flags do have an effect when logging to 
 * files on the device.
 *
 * \param flags Bitwise OR combination of one or more flags, as defined in #fe_log_flag.
 * \see enum fe_log_flag
 */
void fe_log_flags(unsigned long flags);

/*! \brief Atomically get the log module's current flags. 
 *
 * \return The log module's current flags.
 * \see fe_log_flags
 * \see fe_log_flag 
 */
unsigned long fe_log_getflags(void);

/*! \brief Enumeration of log severities for #fe_log_multiplex().
 *
 */
enum fe_log_severity {
    FE_LOG_ALL      = 0,
    FE_LOG_INFO     = 1,
    FE_LOG_WARN     = 2,
    FE_LOG_ERROR    = 3,
    FE_LOG_DEBUG    = 4,
    FE_LOG_VERBOSE  = 5,
    FE_LOG_CRITICAL = 6
};
typedef enum fe_log_severity fe_log_severity;

/*! \brief Prevents messages having the given tag from being displayed to the 
 *         console.
 */
void fe_log_no_console(const char *tag);
/*! \brief Requests that messages having the given tag be displayed to the 
 *         console (using stdout, stderr or Android's Logcat depending 
 *         on cases).
 */
void fe_log_on_console(const char *tag);
/*! \brief Requests that messages having the given tag be also displayed to 
 *         a file.
 *
 * Files are reference_counted within the \c fe_log module, so that no file
 * is fopen()-ed more than once.
 */
void fe_log_file(const char *tag, const char *filename);

/*! \brief Log Infos.
 *
 * This function is for logging regular messages that the user may want to see.
 * <br>
 * The "Info" log is addressed to both developers and end users.
 *
 * The default output stream is \c stdout.\n
 * The default color is green.
 */
void fe_logi(const char *tag, const char *fmt, ...) FE_PRINTF_DECL(2,3);

/*! \brief Log Warnings.
 *
 * Logs a warning, that is, a message describing a potential cause of errors 
 * or undefined behaviour.<br>
 * The "Warning" log is addressed to both developers and end users.
 *
 * The default output stream is \c stderr.\n
 * The default color is yellow.
 */
void fe_logw(const char *tag, const char *fmt, ...) FE_PRINTF_DECL(2,3);

/*! \brief Log Errors.
 *
 * Logs an error, that is, a message describing causes of a situation in which
 * a module (or the app) cannot keep running correctly.<br>
 * The "Error" log is addressed to both developers and end users.
 *
 * The default output stream is \c stderr.\n
 * The default color is red.
 */
void fe_loge(const char *tag, const char *fmt, ...) FE_PRINTF_DECL(2,3);

/*! \brief Log Debug.
 *
 * Logs a debug message, that is, a message which will only be printed by debug
 * builds of the app.<br>
 * The "Debug" log is addressed to anyone running a debug build.
 * Calls to this function are compiled out for non-debug builds.
 *
 * The default output stream is \c stdout.\n
 * The default color is cyan.
 */
void fe_logd(const char *tag, const char *fmt, ...) FE_PRINTF_DECL(2,3);
/*! \brief Valid function pointer to #fe_logd.
 *
 * This function is for when you need to pass #fe_logd as a #fe_logfunc 
 * parameter.
 */
void fe_logd_not_macro(const char *tag, const char *fmt, ...) FE_PRINTF_DECL(2,3);
#ifdef FE_DEBUG_BUILD
#define fe_logd(tag, fmt, ...) 
#endif

/*! \brief Log Verbose.
 *
 * The "Verbose" log is addressed to anyone wanting extremely detailed
 * descriptions of what's happening. It is intended to be disabled most of the
 * time, and especially on release builds.
 *
 * The default output stream is \c stdout.\n
 * The default color is blue.
 */
void fe_logv(const char *tag, const char *fmt, ...) FE_PRINTF_DECL(2,3);
/*! \brief Valid function pointer to #fe_logv.
 *
 * This function is for when you need to pass #fe_logv as a #fe_logfunc 
 * parameter.
 */
void fe_logv_not_macro(const char *tag, const char *fmt, ...) FE_PRINTF_DECL(2,3);

#ifndef FE_LOG_USE_VERBOSE
/*! \brief Dummy macro to compile out calls to #fe_logv() when the 
 *         compile-time switch FE_LOG_USE_VERBOSE is not defined.
 */
#define fe_logv(tag, fmt, ...) 
#endif


/*! \brief Log Critical.
 *
 * Logs a critical message, that is, a message that the end user must 
 * absolutely see.<br>
 * Typically, this function will show a pop-up window. Therefore, it is meant
 * to be used sparingly.
 *
 * It is not necessarily meant for critical errors. It is an 
 * easy way to directly warn the user about anything, from anywhere.
 *
 * Keep in mind, however, that the "true" way to do this is to render logs
 * through games themselves, so they have complete control of what is 
 * displayed to the user and how it is displayed.
 *
 * The default output stream is \c stderr (not counting the pop-up window).\n
 * The default color is red.
 *
 * \see fe_fatal
 */
void fe_logc(const char *tag, const char *fmt, ...) FE_PRINTF_DECL(2,3);

#include <fate/globalstate.h>
#include <fate/sys.h>

/*! \brief Abort on a fatal error.
 *
 * This is a convenience function, since it is roughly equivalent to using
 * #fe_logc, then #fe_sys_log_stacktrace, then freeing resources (the Global
 * State holds references to those) and finally exitting with an error code.
 *
 * However, this is the recommended function to call in extreme cases where
 * there would be no point in leaving the app running anymore.
 *
 * An alternative name would be "fe_panic".
 */
void fe_fatal(const char *tag, const char *fmt, ...) FE_PRINTF_DECL(2,3);
#define fe_fatal(tag, ...) \
    do { fe_sys_log_stacktrace(fe_loge); \
    fe_logc(tag, __VA_ARGS__); \
    fe_globalstate_deinit(fe_gs); \
    exit(EXIT_FAILURE); } while(0)


/*! @} */
#endif /* FE_LOG_H */
