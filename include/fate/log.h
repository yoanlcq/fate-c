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
 * #fate_logc may use SDL_ShowSimpleMessageBox() or a Javascript alert().
 * 
 * Each other module is encouraged to declare a private string which they
 * should pass as the log functions' first parameter, like so :
 * \code{.c}
 * static const char *TAG = "Module name";
 * ...
 * fate_logi(TAG, _("The answer is %u\n"), 42);
 * \endcode
 *
 * In each of the log functions, the "tag" parameter may never be NULL.
 * You may however use an empty string instead.
 *
 * @{
 */

#ifndef FATE_LOG_H
#define FATE_LOG_H

#include <fate/defs.h>
#include <stdarg.h>

/*! \brief Type for the log functions.
 *
 * This is intended for use by other modules, which would like to expose
 * functions taking a log function as a parameter.
 */
typedef void (*fate_logfunc)(const char *tag, const char *fmt, ...);

/*! \brief Setup this module.
 * 
 * This function should be called once to initialize this module.
 * Calling it more than once causes no harm but it is discouraged.
 *
 * This function also calls #fate_log_flags, with #FATE_LOG_COLORED and #FATE_LOG_TAG.
 */
void fate_log_setup(void);

/*! \brief Clean-up this module.
 * 
 */
void fate_log_cleanup(void);


/*! \brief Flags to be given to #fate_log_flags.
 *
 */
enum fate_log_flag {
    FATE_LOG_COLORED  = 1,    /*!< When logging to consoles, attempt to 
        color the output based on the severity (enabled by default). */
    FATE_LOG_DATE     = 1<<1, /*!< Display the date, as defined by the locale.*/
    FATE_LOG_TIME     = 1<<2, /*!< Display the time since the log module was
        set up. */
    FATE_LOG_PID      = 1<<3, /*!< Display the app's processus ID. */
    FATE_LOG_TID      = 1<<4, /*!< Display the current thread ID. */
    FATE_LOG_TICKNO   = 1<<5, /*!< Display the current tick number. */
    FATE_LOG_FRAMENO  = 1<<6, /*!< Display the current frame number.  */
    FATE_LOG_TAG      = 1<<7, /*!< Display the tag (enabled by default).*/
    FATE_LOG_SEVERITY = 1<<8  /*!< Display the severity. Should not be needed 
        if #FATE_LOG_COLORED is raised. */
};
typedef enum fate_log_flag fate_log_flag;

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
 * \param flags Bitwise OR combination of one or more flags, as defined in #fate_log_flag.
 * \see enum fate_log_flag
 */
void fate_log_flags(unsigned long flags);

/*! \brief Atomically get the log module's current flags. 
 *
 * \return The log module's current flags.
 * \see fate_log_flags
 * \see fate_log_flag 
 */
unsigned long fate_log_getflags(void);

/*! \brief Enumeration of log sevreities for #fate_log_multiplex().
 *
 */
enum fate_log_severity {
    FATE_LOG_ALL      = 0,
    FATE_LOG_INFO     = 1,
    FATE_LOG_WARN     = 2,
    FATE_LOG_ERROR    = 3,
    FATE_LOG_DEBUG    = 4,
    FATE_LOG_VERBOSE  = 5,
    FATE_LOG_CRITICAL = 6
};
typedef enum fate_log_severity fate_log_severity;


/*! \brief Atomically multiplexes logs to one or more files at tag-level.
 *
 * Specifies which files the logging functions will write to, for a
 * particular tag.\n
 * As long as this function is not called for a particular tag, then
 * the files used for logging with the tag will be the default ones,
 * that is : \c stdout and \c stderr (\c Logcat on Android).
 *
 * Files given in \p streams <b>must not be closed</b>. They must be treated
 * as "given away" and must be forgotten after the call.\n
 * They are internally reference-counted and are closed when their reference
 * count reaches 0 (exceptions to this rule are \c stdout and \c stderr).
 * One of the following actions causes such reference counts to be decremented :
 * - Calling #fate_log_multiplex() with different files;
 * - Calling #fate_log_cleanup().
 * 
 * By the way, it is safe to provide the same file pointers
 * for different tags and severities.
 *
 * Support for logging to generic I/O streams such as online files and sockets
 * has been considered. However, there's no apparent use case for such a
 * feature, and it just seems overkill. In practice we'll mostly log to the
 * console to get immediate feedback from the app's execution, and at 
 * worst save logs to local files so users can send them back to us.
 *
 * \param tag The tag to multiplex. If NULL, then this sets the default
 *        multiplex files for all tags.
 * \param sev The severity to multiplex, given \p tag. If #FATE_LOG_ALL,
 *        all severities are affected.
 * \param streams Array of extra opened files to use for logging. It can be NULL
 *        as long as \p streams_count is set to zero.
 * \param streams_count Number of elements in \p streams.
 */
void fate_log_multiplex(const char *tag, fate_log_severity sev,
                        FILE* streams[], size_t streams_count);

/*! \brief Log Infos.
 *
 * This function is for logging regular messages that the user may want to see.
 * <br>
 * The "Info" log is addressed to both developers and end users.
 *
 * The default output stream is \c stdout.\n
 * The default color is green.
 */
void fate_logi(const char *tag, const char *fmt, ...);

/*! \brief Log Warnings.
 *
 * Logs a warning, that is, a message describing a potential cause of errors 
 * or undefined behaviour.<br>
 * The "Warning" log is addressed to both developers and end users.
 *
 * The default output stream is \c stderr.\n
 * The default color is yellow.
 */
void fate_logw(const char *tag, const char *fmt, ...);

/*! \brief Log Errors.
 *
 * Logs an error, that is, a message describing causes of a situation in which
 * a module (or the app) cannot keep running correctly.<br>
 * The "Error" log is addressed to both developers and end users.
 *
 * The default output stream is \c stderr.\n
 * The default color is red.
 */
void fate_loge(const char *tag, const char *fmt, ...);

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
#define fate_logd(tag, fmt, ...) \
            fate_log_android(ANDROID_LOG_DEBUG, tag, fmt, __VA_ARGS__); \
            fate_log(tag, FATE_LOG_DEBUG, fmt, __VA_ARGS__);

#ifndef FATE_DEBUG_BUILD
/*! \brief Dummy macro to compile out calls to #fate_logd() when not in a debug 
 *         build.
 */
#define fate_logd(tag,fmt) 
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
#define fate_logv(tag, fmt, ...) \
            fate_log_android(ANDROID_LOG_VERBOSE, tag, fmt, __VA_ARGS__); \
            fate_log(tag, FATE_LOG_VERBOSE, fmt, __VA_ARGS__);

#ifndef FATE_LOG_USE_VERBOSE
/*! \brief Dummy macro to compile out calls to #fate_logv() when the 
 *         compile-time switch FATE_LOG_USE_VERBOSE is not defined.
 */
#define fate_logv(tag,fmt) 
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
 * \see fate_fatal
 */
#define fate_logc(tag, fmt, ...) \
            (fate_log_android(ANDROID_LOG_ERROR, tag, fmt, __VA_ARGS__), \
            fate_log(tag, FATE_LOG_CRITICAL, fmt, __VA_ARGS__))

/*! \brief Abort on a fatal error.
 *
 * This is a convenience function, since it is roughly equivalent to using
 * #fate_logc, then #fate_sys_log_stacktrace, then freeing resources (the Global
 * State holds references to those) and finally exitting with an error code.
 *
 * However, this is the recommended function to call in extreme cases where
 * there would be no point in leaving the app running anymore.
 *
 * An alternative name would be "fate_panic".
 */
#define fate_fatal(tag, fmt, ...) \
            (fate_logc(tag, fmt, __VA_ARGS__), \
             fate_fatal_real(tag, fmt, __VA_ARGS__))



/*! @} */
#endif /* FATE_LOG_H */
