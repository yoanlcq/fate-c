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

/*! \file fate/promise.h
 *  \brief Promises and Futures.
 *  \defgroup promise Promises and Futures.
 *
 * <b><i>This page is must-read for internal devs.</i></b>
 *
 * This module is very important to F.A.T.E since its whole execution model
 * revolves around task graphs. They are a very efficient and scalable way
 * to make the best of today's CPUs, as well as Javascript asynchronous
 * calls on Emscripten.
 *
 * Promises and futures are the interface to said task graphs, and they
 * implement common asynchronous operations.
 *
 * <h2>Background</h2>
 * <h3>The Task Graph pattern</h3>
 * Task Graphs are a great way to develop multithreaded applications in a
 * scalable way.
 *
 * As described in Intel's Dont Dread Threads, a _task_ is the smallest unit of
 * work in the program's pipeline. By definition, a task can either fail
 * or succeed, but never be suspended.
 *
 * Actually, tasks are here simply implemented as function calls.
 *
 * At startup, the program launches a pool of \c N threads, \c N 
 * being chosen to match the amount of parallelism the end user's hardware
 * provides.<br>
 * That's the theory. In practice, Emscripten does not support threads.
 * Also, we let configuration files determine the number of threads the
 * pool should hold.<br>
 * But the point is, the number of threads does not imply any change in source
 * code. In the same way, source code is not required to be tuned for a
 * given number of threads.
 *
 * So then, the program defines what tasks should be run when some others
 * complete, effectively building one or more Task Graphs.<br>
 * Tasks are queued, and each thread's job is to repeatedly pop tasks from 
 * the queue. If the queue is empty, they just wait.
 *
 * It's easy to see why this is efficient. It's so great that Crytek moved
 * their engine from a serial implementation to task-based scheduling. 
 * UE4 provides this. Promises are a long-awaited feature of ECMAScript 6.
 * Task Graphs are one of Intel's Thread Building Blocks's main selling points.
 *
 * In the future, we can only expect more and more parallelism from the end
 * user's harware, so choosing how a game engine will implement 
 * multithreading is a critical decision 
 * <i>that must be taken from the start</i>.
 *
 * <h3>When \c fopen() is not enough anymore</h3>
 * What actually led me to implement promises in the first place is 
 * Emscripten's way of managing files.
 *
 * With Emscripten, your app runs on a browser and the user is not expected to
 * already have all of your assets downloaded (otherwise it is just as
 * interesting as downloading the game's desktop/mobile version).
 *
 * You can "embed" files into your app (which will be available to
 * \c fopen() and friends), but the downside is that they will be loaded
 * during Emscripten's preload phase, which leaves the user in front of
 * a blank screen until the downloads are completed.<br>
 * I wouldn't expect such a thing from actual games. I would expect having to
 * wait a few seconds before getting a loading screen that is, at worst, 
 * super cool, and at best, telling me everything that's happening while I'm 
 * waiting.<br>
 * I also think the preloading phase has no reason to be separated from the 
 * game. Required files should be downloaded asynchronously while the game
 * already starts showing animations and stuff.
 *
 * Since Emscripten provides <tt><a href="https://kripken.github.io/emscripten-site/docs/api_reference/emscripten.h.html#c.emscripten_async_call" target="_blank">emscripten_async_call()</a></tt> and <tt><a href="https://kripken.github.io/emscripten-site/docs/api_reference/emscripten.h.html#c.emscripten_async_wget" target="_blank">emscripten_async_wget()</a></tt>,
 * I figured the whole engine's pipeline needed to be asynchronous.
 *
 * <h3>Promises and futures</h3>
 *
 * <h2>Problems solved by this module</h2>
 * The global state holds (doubly ?) linked lists of tasks, called <b>Task lists</b>.<br>
 *
 * For example, starting a file promise does not internally push a big "download file" task.<br>
 * Instead, it pushes several small "download a piece of that file" tasks, each dependant from the previous.
 *
 * A task may be given hints, such as a priority and an estimated completion time.<br>
 * Of course, those hints need not be precise, but they help 
 * balancing load between threads properly.
 *
 * Each task list plays a <b>role</b> in the program's execution. Threads subscribe
 * to one or more lists depending on their privileges.<br>
 * (Typically, only the main thread will subscribe to the "Window Events list"
 * just like the only thread responsible for the OpenGL context will subscribe
 * to the "Video list").<br>
 * See #fate_task_role to know more about this.
 * 
 * If profiling is enabled, the thread reports statistics, such as, how much time it has spent waiting,
 * the number of times it has waited, 
 * how much time it has spent executing each task,
 * which role that task was in, etc.
 *
 * <h2>How to use this module</h2>
 *
 *
 * <h2>FAQ/discussion</h2>
 * <b>It's so huge ! What's the true reason behind the whole thing ?</b><br>
 * One word : Emscripten.<br>
 * Normally it leaves the user in front of a dumb loading bar during the 
 * preloading phase, while each file you've decided to "embed" is being
 * downloaded.<br>
 * That's how promises arised. They are not intended to be regular objects.
 * They are intended to be the whole program.<br>
 * You do not need anymore to be _certain_ that your task will execute.
 * You only need to be _confident_ about it. That's what promises are about,
 * and it's way better.
 *
 * <b>Is the overhead of managing a task graph worth it ? What happened to
 * good old serial execution ?</b><br>
 * We must forget Good Old Serial Execution because it implies everything 
 * is _synchronous_. But if we're single-threaded, then task graphs will
 * be run serially, can't help it.\n
 * That's still cool though, because now everything is balanced !\n
 * Thanks to promises, even when single-threaded, not only we don't block
 * anymore, but workload is balanced. A bit of everything is done on a
 * given period of time, instead of \c "WAIT FOR THE FILE. DOWNLOAD THE FILE. 
 * OOPS I MISSED THE FRAME".
 *
 * <b>That really feels like reinventing flow control !</b><br>
 * Yes. But you could say that promises are the future of 
 * programming.
 *
 * <b>Am I forced to rebuild the graph each time to restart it ?</b><br>
 *
 * <b>Can't we then go further by providing compile-time task graphs ?</b><br>
 *
 * <b>Couldn't task graphs loop, perhaps even replacing the whole 
 * program ?</b><br>
 * Yes.
 *
 * <b>Should we provide task graph objects, that hold their own set of
 * promises ? Such task graph objects could be individually profiled !</b><br>
 * No. It would be annoying as hell.<br>
 * Promises are not just here to provide casual asynchronous calls. 
 * They are a whole way to organise programs.<br>
 * Creating separate "task graphs" should be as easy a starting a set 
 * of promises that don't share any dependency.<br>
 * Anyway, from to caller's point of view, having to create and manage
 * such task graphs just looks dumb.<br>
 * Look at C# and ECMAScript 6 : there's no such thing.<br>
 * As for profiling, we usually want to profile everything at once.<br>
 * Plus, it's not like we really need to instanciate task graphs : A 
 * well-designed program should be representable by a single looping 
 * task graph.
 *
 * <b>Aren't there too many task categories ?</b><br>
 * No. I even have a feeling there would be more in the future.
 *
 * <b>I'm a task. Can I split myself into sub-tasks ?</b><br>
 *
 * <b>Reference counting for data flow between tasks ?</b><br>
 *
 * <b>Why is there no \c catch() ?</b><br>
 *
 * @{
 */



#ifndef FATE_FUTURE_H
#define FATE_FUTURE_H

#include <stdbool.h>

/*
typedef void (*em_str_callback_func)(const char *);
typedef void (*em_async_wget2_data_onload_func)(void *arg, void *buf, unsigned *size);
typedef void (*em_async_wget2_data_onerror_func)(void *arg, int http_code, const char *description)
typedef void (*em_async_wget2_data_onprogress_func)(void *arg, int bytes_loaded, int total_size);

void emscripten_wget(const char* url, const char* file);
void emscripten_async_wget(const char* url, const char* file, em_str_callback_func onload, em_str_callback_func onerror);
int emscripten_async_wget2_data(const char* url, const char* requesttype, const char* param, void *arg, int free, em_async_wget2_data_onload_func onload, em_async_wget2_data_onerror_func onerror, em_async_wget2_data_onprogress_func onprogress)
void emscripten_async_wget2_abort(int handle);

typedef void (*em_async_wget_onload_func)(void *arg, void *buf, int size);
typedef void (*em_arg_callback_func)(void *arg);
void emscripten_idb_async_load(const char *db_name, const char *file_id, void* arg, em_async_wget_onload_func onload, em_arg_callback_func onerror)
void emscripten_idb_async_store(const char *db_name, const char *file_id, void* ptr, int num, void* arg, em_arg_callback_func onstore, em_arg_callback_func onerror);
*/

struct fate_promise;
/*! \brief Opaque object holding both a promise and its future. 
 *
 * Promise/future objects are stored in a internal pool, so their 
 * lifetime is preserved as long as they are needed.<br>
 * You only get pointers ("handles") to them.

 * Here are the steps to use a #fate_promise object :
 * -# Create one;
 * -# Bind it to a target (e.g task, file, fsync or smtp);
 * -# Optionally, you can repeat the previous step, which will
 *    override the previous binding;
 * -# Depending on what the promise is bound to, and your preferences, 
 *    configure your promise through various setters, for which there should
 *    be getters;
 * -# Start the promise;
 * -# Do some other work. You can also periodically check the promise's progress;
 * -# The result is called a _future_. So you wait on the future. Once it has
 *    arrived, attempting to wait on it again will simply return it.
 *
 * At any step, you may decide you don't care about the promise anymore,
 * so you must close its future. Depending on the promise's status, other
 * actions may be taken so the
 * <a href="https://en.wikipedia.org/wiki/Principle_of_least_astonishment" 
 * target="_blank">principle of least suprise</a> is respected.\n
 * For instance, if you call #fate_future_close() after writing to a file
 * you have got via a file promise, you expect the file's changes to be 
 * committed to the storage, just like with \c fclose().\n
 * To learn more about this behaviour, or if you want to change it, 
 * see #fate_promise_bind_fsync() and #FATE_PROMISE_FSYNC_DIRTY_FLAG.
 *
 * Here is an example usage :
 * \code{.c}
 * #include <fate/future.h>
 *
 * fate_promise *file_promise = fate_promise_create();
 * fate_promise_bind_file(FATE_STORAGE_FTP, "me@foobar.net:21/hello.txt", "w+");
 * fate_promise_set_str(FATE_PROMISE_FTP_LOGIN, "me", FATE_NOT_MALLOCED);
 * // Did you think you'd get my password so easily ?
 * fate_promise_set_str(FATE_PROMISE_FTP_PASSWD, "*****", FATE_NOT_MALLOCED); 
 * // Uncomment the following if you want to use SFTP.
 * // fate_promise_set_u32(FATE_PROMISE_FTP_IS_OVER_SSH, 1);
 * // Uncomment the following if you want another port than the default.
 * // fate_promise_set_u32(FATE_PROMISE_NETPORT, i_dunno);
 * fate_promise_start(file_promise);
 * do_some_stuff();
 * // OBVIOUS TODO
 * fate_future_close(file_promise);
 * \endcode
 *
 */
typedef struct fate_promise fate_promise;

/*! \brief Error codes common to most non-user broken promises.
 */
enum fate_promise_error {
    FATE_PROMISE_ERR_NOERROR = 0, /*!< No error. */
    FATE_PROMISE_ERR_UNKNOWN, /*!< Unknown error. */
    FATE_PROMISE_ERR_NOENT, /*!< No such file or directory. */
    FATE_PROMISE_ERR_ISDIR /*!< You wanted to open a file, but 
                             it's a directory. */
};
/*! \brief TODO
 */
typedef enum fate_promise_error fate_promise_error;


/*! \brief Task categories, used for scheduling.
 */
enum fate_task_role {
    FATE_TASK_MISC = 0, /*!< Miscellaneous.<br>
    * This role's color for visual profiling is <span style="color:magenta;">Magenta</span>.<br>
    * Any task that doesn't neatly fit the other categories should run under this one. */
    FATE_TASK_WINDOW = 1, /*!< Only the thread(s) that can manage windows 
    * and window events will pick tasks under that role.<br>
    * Chances are it is the main thread.<br>
    * This role's color for visual profiling is <span style="color:cyan;">Cyan</span>.<br>
    */
    FATE_TASK_VIDEO = 2, /*!< Only the thread(s) responsible for the 
    * OpenGL (or other) rendering context(s) will pick tasks under that role.<br>
    * This role's color for visual profiling is <span style="color:orange;">Orange</span>.<br>
    */
    FATE_TASK_AUDIO = 3, /*!< Only the thread(s) responsible for the 
    * OpenAL (or other) audio context(s) will pick tasks under that role.<br>
    * This role's color for visual profiling is <span style="color:yellow;">Yellow</span>.<br>
    */
    FATE_TASK_FILEIO = 4, /*!< Local file input/output.<br>
    * This role's color for visual profiling is <span style="color:green;">Green</span>.<br>
    * Any thread can pick tasks under that role, 
    * with more or less probability depending on its responsibilities. */
    FATE_TASK_NETWORKIO = 5, /*!< Network input/output.<br>
    * This role's color for visual profiling is <span style="color:red;">Red</span>.<br>
    * Any thread can pick tasks under that role, 
    * with more or less probability depending on its responsibilities. */
    FATE_TASK_COMPUTE = 6 /*!< Any thread can pick tasks under that role, 
    * with more or less probability depending on its responsibilities.
    * <br>This is quite like #FATE_TASK_MISC, except the task is rather expected to actually be a computation.<br>
    * This role's color for visual profiling is <span style="color:blue;">Blue</span>.<br>
    */
};
/*! \brief Task categories, used for scheduling.
 */
typedef enum fate_task_role fate_task_role;


/*! \brief Definition for an asynchronous task given to 
 *         #fate_promise_bind_task().
 *
 * The task returns whatever it wants through a pointer.
 *
 * The task is given its #fate_promise object so it can (and should) :
 * - Call #fate_promise_get_str(#FATE_PROMISE_TASK_DATA) on it (if expected);
 * - Call #fate_promise_set_u32(#FATE_PROMISE_PROGRESS) on it;
 * - Call #fate_promise_set_u32(#FATE_PROMISE_ERROR) on it;
 * - Call #fate_promise_set_str(#FATE_PROMISE_ERROR) on it.
 * 
 * When the task function returns, its promise's progress is automatically set 
 * to \c 1.\n
 *
 * \param future A #fate_promise object so the task can indicate its progress or set its error.
 * \param arg A user parameter given to fate_promise_task.
 * \return NULL or a dynamically-allocated buffer (like with malloc()).
 * \see fate_promise_task
 */
typedef void (*fate_taskfunc)(fate_promise *promise);

/*! \brief A type for holding a file interaction mode.
 *
 * On all modes, a \c "b" should be appended when dealing with binary data.\n
 * It is ignored with the \c "x", "d", "de" and "dr" modes.
 *
 * File promises using a write-only mode (i.e \c "w", \c "wx", 
 * \c "a" and \c "ax") should return their future faster, since there's
 * no need to download the file. See #fate_promise_bind_fsync() to learn
 * more about how the file is committed.
 *
 * Valid values are :
 * - \c "x"   - Just check if a file or directory exists.\n
 *              #fate_future_wait_file() will return \c NULL.\n
 *              You must then get the result by calling
 *              #fate_promise_getu32() with #FATE_PROMISE_ERROR,
 *              then compare it to the following values :
 *              - #FATE_PROMISE_ERR_NOENT   - No such file or directory.
 *              - #FATE_PROMISE_ERR_NOERROR - The entry exists, and it's a file.
 *              - #FATE_PROMISE_ERR_ISDIR   - The entry exists, and it's a 
 *                                            directory.
 * - \c "d"   - Delete a file, not a directory.
 * - \c "de"  - Delete a file or an empty directory.
 * - \c "dr"  - Delete a file, or a directory with all of its contents.
 * - \c "r"   - Open file for reading.
 * - \c "r+"  - Open file for reading and writing.
 * - \c "w"   - Open file for writing.
 *              The file is created if it does not exist or truncated if 
 *              it exists.
 * - \c "wx"  - Like \c "w" but fails if path exists.
 * - \c "w+"  - Like \c "w" but also allow reading.
 * - \c "wx+" - Like \c "w+" but fails if path exists.
 * - \c "a"   - Open file for appending. 
 *              The file is created if it does not exist.
 * - \c "ax"  - Like \c "a" but fails if path exists.
 * - \c "a+"  - Like \c "a" but also allow reading.
 * - \c "ax+" - Like \c "a+" but fails if path exists.
 *
 * \see fate_promise_bind_file
 * \see fate_promise_bind_fsync
 */
typedef const char* fate_file_op_string;

/*! \brief A type for indicating where to load/store a file. 
 *
 *  \see fate_promise_bind_file 
 */
enum fate_storage {
    FATE_STORAGE_FILESYSTEM = 1, /*!< The file system, no matter the platform.
    * <br>On Emscripten, it is MEMFS, which means the files must be packaged 
    * with emcc's --preload-file option. */
    FATE_STORAGE_RESOURCE = 2, /*!< The resource storage 
    (usually for asset data).
    - On Emscripten, it is the application's server.
    - On mobile devices, it is the app's resource directory.
    - Else, it is the file system. */
    FATE_STORAGE_PERSISTENT = 3, /*!< The platform's persistent storage 
    (usually for user-specific data).
    - On Emscripten, it is IndexedDB.
    - On mobile devices, it is the platform-specific persistent storage.
    - Else, it is the file system. */
    FATE_STORAGE_PERSISTENT_EXTERNAL = 4, /*!< Just like
    #FATE_STORAGE_PERSISTENT, except that, on mobile devices,
    the external storage if preferred when available (e.g the SD card). 
    See #FATE_STORAGE_EXTERNAL_BYTES_LEFT to see if external storage
    is available. If it is, you'll get approximately how many bytes you can
    use for storing data. */
    FATE_STORAGE_HTTP = 5, /*!< The Web, no matter the platform, 
    through HTTP with GET and POST.  
    <br>There's no way to choose a port other than the default, 
    since Emscripten doesn't allow it. */
    FATE_STORAGE_FTP = 6 /*!< FTP, no matter the platform. 
    See also the \c FATE_PROMISE_FTP_* acessors enumerated in 
    #fate_promise_attr. */
};
/*! \brief TODO */
typedef enum fate_storage fate_storage;

/*!\brief Possible value for #FATE_PROMISE_TYPE. */
enum fate_promise_type {
    FATE_PROMISE_TYPE_NONE = 0, /*!< TODO */
    FATE_PROMISE_TYPE_TASK,     /*!< TODO */ 
    FATE_PROMISE_TYPE_FILE,     /*!< TODO */
    FATE_PROMISE_TYPE_FSYNC,    /*!< TODO */
    FATE_PROMISE_TYPE_SMTP,     /*!< TODO */
};
/*! \brief TODO */
typedef enum fate_promise_type fate_promise_type;

/*! \brief TODO */
enum fate_promise_attr {
    FATE_TASKGRAPH_NUM_THREADS = 0, /*!< u32 - Get/Set -  The 
                                      number of threads used. */
    FATE_TASKGRAPH_POOL_HINT, /*!< u32 - Get/Set - help 
                               preallocating promises. */
    FATE_PROMISE_TYPE, /*!< u32 - Get - TODO*/
    FATE_PROMISE_THREADNO_HINT, /*!< u32 - Get/Set - TODO */
    FATE_PROMISE_PRIORITY, /*!< u32 - Get/Set - TODO */
    FATE_PROMISE_PRIORITY_HINT, /*!< u32 - Get/Set - TODO */
    FATE_PROMISE_COMPLETION_TIME_HINT, /*!< u32 - Get/Set - TODO */
    FATE_PROMISE_ERROR, /*!< u32/str - Get/Set - TODO */
    FATE_PROMISE_STATUS, /*!< u32 - Get */
    FATE_PROMISE_PROGRESS, /*!< f32/str - Get/Set - TODO */
    FATE_PROMISE_NUM_THENS, /*!< u32 - Get */
    FATE_PROMISE_NUM_DEPS, /*!< u32 - Get */
    FATE_PROMISE_TASKGRAPH_RESTART_POINT, /*!< u32 - Get/Set - 
        Maybe OBSOLETE ?? The main thread should just do :
        \code{.c}
        while(run_taskgraph());
        \endcode
        But maybe it is required, if we decouple the frame point and the
        tick point. Hence #FATE_PROMISE_TASK_IS_FRAME and
        #FATE_PROMISE_TASK_IS_TICK, after all.
        */ 
    FATE_PROMISE_TASK_IS_FRAME, /*!< u32 - Get/Set - TODO 
        Gives a certain priority. */
    FATE_PROMISE_TASK_IS_TICK, /*!< u32 - Get/Set - TODO */
    FATE_PROMISE_TASK_DATA, /*!< ptr - Get/Set - TODO */
    FATE_PROMISE_TASK_FUNC, /*!< ptr - Get/Set - TODO */
    FATE_STORAGE_EXTERNAL_BYTES_LEFT, /*!< u32 - Get */
    FATE_PROMISE_FILE_STORAGE, /*!< u32 - Get - TODO */
    FATE_PROMISE_FILE_LAZY, /*!< u32 - Get/Set - TODO 
        The default behaviour (zero) only makes the future available
        when the file has been entirely downloaded. This behaviour should
        be chosen for smaller files.
        If non-zero, the future is nearly instantly available; 
        Parts of the file are downloaded as the future is read.
        This value sets the chunk size.
        */
    FATE_PROMISE_FILE_URL, /*!< str - Get - TODO */
    FATE_PROMISE_FILE_OP, /*!< str - Get - TODO */
    FATE_PROMISE_FILE_SERVER_TIMEOUT, /*!< u32 - Get/Set - TODO */
    FATE_PROMISE_FILE_BYTES_LOADED,  /*!< u32 - Get/Set - TODO */
    FATE_PROMISE_FILE_BYTES_TOTAL, /*!< u32 - Get/Set - TODO */
    FATE_PROMISE_NETPORT,  /*!< u32 - Get/Set - Set it to 0 for the default. */
    FATE_PROMISE_FTP_LOGIN, /*!< str - Get/Set - TODO */
    FATE_PROMISE_FTP_PASSWD, /*!< str - Get/Set - TODO */
    FATE_PROMISE_FTP_IS_OVER_SSH, /*!< u32 - Get/Set - TODO */
    FATE_PROMISE_FSYNC_DIRTY_FLAG, /*!< u32 - Get/Set - TODO */
    FATE_PROMISE_SMTP_DOMAINNAME, /*!< str - Get/Set - TODO */
    FATE_PROMISE_SMTP_FROM, /*!< str - Get/Set - TODO */
    FATE_PROMISE_SMTP_REPLYTO, /*!< str - Get/Set - TODO */
    FATE_PROMISE_SMTP_TO, /*!< str - Get/Set - TODO */
    FATE_PROMISE_SMTP_CC, /*!< str - Get/Set - TODO */
    FATE_PROMISE_SMTP_BCC, /*!< str - Get/Set - TODO */
    FATE_PROMISE_SMTP_SUBJECT, /*!< str - Get/Set - TODO */
    FATE_PROMISE_SMTP_BODY, /*!< str - Get/Set - TODO */
    FATE_PROMISE_SMTP_MESSAGEID /*!< str - Get/Set - Automatically generated 
                                   by default. */
};
/*! \brief TODO */
typedef enum fate_promise_attr fate_promise_attr;

#ifdef __EMSCRIPTEN__
#define fate_sleep_with_yield(ms) emscripten_sleep_with_yield(ms)
#else
/*! \brief Yields some time for promises to run.
 *
 * It is possible to achieve an active wait by calling repeatedly 
 * #fate_sleep_with_yield() and 
 * #fate_promise_get_f32(#FATE_PROMISE_PROGRESS). */
void fate_sleep_with_yield(unsigned ms);
#endif

/*! \brief Creates a promise object.
 *
 * Delete it with #fate_future_close().
 * You can help the internal pool making decisions for underlying allocations
 * with #FATE_TASKGRAPH_POOL_HINT.
 */
fate_promise* fate_promise_create(void);

/*! \brief Fills the given array with promise object pointers.
 *
 * blah
 */
void fate_promise_createv(fate_promise* promises[], size_t count);


/*! \brief Creates a file promise for opening, checking or deleting a file.
 * 
 * \p url and \p op are copied into a dynamically-allocated buffer..
 *
 * Internally, this function adds one or more tasks to the "File IO" 
 * or "Network IO" task lists (see #fate_task_role for more info).
 *
 * Please refer to the links below.
 *
 * \see fate_promise
 * \see fate_storage
 * \see fate_file_op_string
 * \see fate_future_wait_file 
 */
fate_promise* fate_promise_bind_file(fate_storage storage, 
                            const char *url, fate_file_op_string op);


/*! \brief Creates a task promise, for... executing a task (Duh !).
 * 
 * Intel TBB makes the distinction between data flow graphs and dependance
 * graphs. F.A.T.E doesn't, leaving it up to the caller.
 *
 * Please refer to the links below.
 * \see fate_promise
 * \see fate_taskfunc
 */
void fate_promise_bind_task(fate_promise *p, fate_task_role role,
                            fate_taskfunc task);
/*! \brief TODO 
 *
 * blah */
void fate_promise_bind_fsync(fate_promise *p, fate_promise *file_future);
/*! \brief TODO 
 *
 * blah */
void fate_promise_bind_smtp(fate_promise *p);
/*! \brief TODO*/
void fate_promise_start(fate_promise *p);

/*! \brief TODO
 *
 * Blah.
 */
void fate_promise_set_u32(fate_promise *p, uint32_t val);
/*! \brief TODO
 *
 * All of the above
 */
uint32_t fate_promise_get_u32(fate_promise *p);
/*!\brief TODO
 *
 * blah
 */
void fate_promise_set_f32(fate_promise *p, float val);

/*! \brief TODO 
 *
 * blah
 */
float fate_promise_get_f32(fate_promise *p, float val);

/*! \brief TODO */
enum fate_malloc_type {
    FATE_NOT_MALLOCED = 0, /*!< TODO */
    FATE_STD_MALLOCED = 1, /*!< TODO */
    FATE_MEM_MALLOCED = 2, /*!< TODO */
};
/*! \brief TODO */
typedef enum fate_malloc_type fate_malloc_type;
/*! \TODO sets
 *
 * See RFC 5321 and RFC 5322.
 * ---
 * S: 220 smtp.example.com ESMTP Postfix
 * C: HELO relay.example.org 
 * S: 250 Hello relay.example.org, I am glad to meet you
 * C: MAIL FROM:<bob@example.org>
 * S: 250 Ok
 * C: RCPT TO:<alice@example.com>
 * S: 250 Ok
 * C: RCPT TO:<theboss@example.com>
 * S: 250 Ok
 * C: DATA
 * S: 354 End data with <CR><LF>.<CR><LF>
 * C: From: "Bob Example" <bob@example.org>
 * C: To: "Alice Example" <alice@example.com>
 * C: Cc: theboss@example.com
 * C: Date: Tue, 15 January 2008 16:02:43 -0500
 * C: Subject: Test message
 * C: 
 * C: Hello Alice.
 * C: This is a test message with 5 header fields and 4 lines in the message body.
 * C: Your friend,
 * C: Bob
 * C: .
 * S: 250 Ok: queued as 12345
 * C: QUIT
 * S: 221 Bye
 * {The server closes the connection}
 * ---
 * From: "Joe Q. Public" <john.q.public@example.com>
 * Subject: Hi everyone!
 * Cc: <boss@nil.test>, "Giant; \"Big\" Box" <sysservices@example.net>
 * Date: Tue, 1 Jul 2003 10:52:37 +0200
 * Message-ID: <5678.21-Nov-1997@example.com>
 *
 * Hi everyone.
 * ---
 *
 * \p dynamic instructs the module whether the string should be freed before
 * subsequent calls to setstr() or future_close().
 */
void fate_promise_set_str(fate_promise *p, char *str, 
                          fate_malloc_type malloced);
/*! \brief TODO The caller must NOT free the string here. 
 *
 * blah
 */
const char *fate_promise_get_str(fate_promise *p);

/*! \brief TODO 
 *
 * blah
 */
void fate_promise_set_ptr(fate_promise *p, void *ptr, 
                          fate_malloc_type malloced);
/*! \brief TODO The caller must NOT free the memory here. 
 *
 * blah
 */
const void *fate_promise_get_ptr(fate_promise *p);
/*! \brief TODO 
 *
 * blah
 */

void fate_promise_then(fate_promise *dep, fate_promise *then);
/*! \brief Says "I don't depend on this promise anymore". 
 *
 * It is not really deprecated, but rather really discouraged. 
 * The dep will remain in the promise's dependency list (so its thens may still 
 * use its result, but will indeed by marked as "dropped" for this promise,
 * meaning it won't require the dep to complete before starting anymore. */
void fate_promise_drop_dep(fate_promise *p, size_t dep_index) FATE_DEPRECATED;
/*! \brief TODO 
 *
 * blah
 */
fate_promise* fate_promise_get_then(fate_promise *dep, size_t index);
/*! \brief TODO 
 *
 * blah
 */
fate_promise* fate_promise_get_dep(fate_promise *then, size_t index);



/*! \brief Waits for any future.
 * 
 */
void fate_future_wait(fate_promise *p);


/*! \brief Waits for the file future, and returns an opened file, or \c NULL 
 *         if there was an error.
 *
 * Make sure it is not \c NULL.
 *
 * Do not \c fflush() the file. Use #fate_promise_bind_fsync() instead.<br>
 * Do not \c fclose() the file. Use #fate_future_close() instead. 
 */
SDL_RWops *fate_future_wait_file(fate_promise *future);



/*! \brief Discards unstarted promises, Aborts a running one or releases 
 *         its future.
 * 
 * If the future is not available yet, the promise is cancelled 
 * and resources are released.<br>
 * Else, the future's resources are released.
 * (This is where free() and/or fclose() are called).
 *
 * If the promise is part of a task graph, its deletion and removal from the 
 * graph is deferred until all its tasks are complete.
 * The promise is also disconnected from the graph.
 * 
 * \p future is not reusable and must be forgotten.
 *
 * For file futures, an implicit #fate_promise_bind_fsync() promise 
 * is performed synchronously before releasing resources.<br>
 * Therefore, it is recommended, for online files, to first call
 * #fate_promise_bind_fsync() in order to handle the upload's progress. */
void fate_future_close(fate_promise *future);

/*! @} */

#endif /* FATE_FUTURE_H */
