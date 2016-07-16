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

/*! \file fate/mem/stack.h
 *  \brief Safe and unsafe Stack allocation routines.
 *  \ingroup mem
 *
 * @{
 */

#ifndef FE_MEM_STACK_H
#define FE_MEM_STACK_H

#include <stddef.h>
#include <fate/defs.h>

/*! \brief Defined only if this module's stack allocation routines can actually
 *         allocate on the stack (because otherwise, they fall back to heap
 *         allocation). As such, you are not actually required to use it for
 *         conditional compilation.
 *
 * This symbol expands to 0 if the current compiler/platform combination
 * provides no way at all to perform stack allocation.
 */
#define FE_MEM_STACK_ALLOCATION_IMPLEMENTED 1

/*! \brief Pretty self-explanatory. This is platform-specific.
 *
 * It expands to :
 * - \c (-1) if the stack is known to grow downwards;
 * - \c 1 if the stack is known to grow upwards;
 * - \c 0 otherwise.
 */
#ifdef FE_HW_X86
#define FE_MEM_STACK_GROWTH_DIRECTION (-1)
#else
#define FE_MEM_STACK_GROWTH_DIRECTION 0
#endif

/*! \brief Atomically attempts to allocate memory on the stack.
 *         <b>Please read the description carefully</b>.
 * 
 * Here is what you should know about stack allocation :
 * - It's crazy fast, compared to heap allocation (e.g with \c malloc());
 * - Since it's not on the heap, it can help avoiding virtual memory
 *   fragmentation in multithreaded programs;
 * - Stack size is limited by the OS, and knwo to be small,
 *   so it should never be used to allocate big chunks of data;
 * - On some platforms, stack size can be grown on-demand (eg. Linux);
 * - Data is automatically freed as soon as the function you are in returns,
 *   <b>but not if the heap is used as a fallback (see below)</b>.
 *
 * This function attempts to allocate \p size bytes on the stack, but falls
 * back to regular heap allocation if at least one of the following 
 * conditions are met :
 * - No stack allocator (such as \c alloca() or \c _malloca()) is provided 
 *   for the current compiler or platform 
 *   (see #FE_MEM_STACK_ALLOCATION_IMPLEMENTED);
 * - There is no way to get enough information to prevent stack overflows
 *   (see #fe_mem_stackavail());
 * - The above conditions are not met, but it is either known or assumed 
 *   that the allocation would cause a stack overflow.
 *
 * #fe_mem_stackfree() is provided because of such potential fallbacks.<br>
 * The caller must use it when they are done with the allocated memory.<br>
 * It will be a no-op if the memory has indeed been allocated from the 
 * stack, or a call to \c free() otherwise.
 *
 * Please conform to the following guidelines :
 * - Never assume that memory is freed when going out of the current function's 
 *   scope;
 * - Never assume that memory remains available outside the current function's 
 *   scope;
 * - Therefore, restrict related calls to #fe_mem_stackalloc() 
 *   and #fe_mem_stackfree() to <b>the same function's scope</b>;
 * - If you are confident that your allocation won't cause a stack overflow,
 *   you may use #fe_mem_stackalloc_confident() which won't perform any checks.
 *
 * <b>Implementation detail</b> : If required, #fe_mem_stackalloc() may 
 * allocate a few <b>extra bytes</b> right before the returned address.
 * The said byte would be used internally by #fe_mem_stackfree() so it 
 * can take the appropriate decision.<br>
 * Please take this into account when estimating whether there is enough stack 
 * space left, but you can <b>never</b> rely on their actual presence.
 *
 * Bad usage example :
 * \code{.c}
 * #include <stddef.h> //For NULL
 * #include <fate/log.h>
 * #include <fate/mem.h>
 *
 * static const char *TAG = "Bad example";
 * #define LEN 4
 * static int *stored;
 * 
 * static void fill(void) {
 *     size_t i;
 *     stored = fe_mem_stackalloc(LEN*sizeof(*stored));
 *     if(!stored)
 *         fe_fatal(TAG, "Oops\n");
 *     for(i=0 ; i<LEN ; ++i)
 *         fe_logi(TAG, "%d\n", stored[i]);
 *
 *     // The mistake is right here :
 *     // 1. Failure to call fe_mem_stackfree();
 *     // 2. 'stored' is invalid as soon as this function returns, and thus 
 *     //    must not be accessed outside of this function's scope.
 * }
 * 
 * static void display(void) {
 *     size_t i;
 *     for(i=0 ; i<LEN ; ++i) // *Undefined behaviour intensifies*
 *         fe_logi(TAG, "%d\n", stored[i]); 
 *     fe_logi(TAG, "\n");
 * }
 * 
 * int main(int argc, char *argv[]) {
 *     fe_log_setup();
 *     fe_mem_setup();
 *     fill();
 *     display();
 *     return 0;
 * }
 * \endcode
 *
 * Better usage example :
 * \code{.c}
 * void this_function_is_kind(void) {
 *     int *values = fe_mem_stackalloc(2*sizeof(int));
 *     // Check here if 'values' is NULL, and handle the error too;
 *     // Do your stuff with it otherwise;
 *     fe_mem_stackfree(values); //Then don't forget.
 * }
 * \endcode
 * 
 * Just for future reference, here are some possible stack allocators :
 * \code{.c}
 * // GCC
 * __builtin_alloca(size);
 * 
 * // Windows
 * _malloca(size);
 * _freea(ptr);
 *
 * // Most other platforms
 * #include <alloca.h>
 * alloca(size);
 * \endcode
 *
 * \param size Requested memory size in bytes. 0 is fine 
 *             (for testing the returned value in \p left), but in 
 *             such a case, for obvious reasons, you must <b>not</b> attempt 
 *             to access the returned memory.
 * \return Pointer to \p size uninitialized bytes that may be used to store 
 *         data within the current function's scope, or NULL on failure.
 * \see fe_mem_stackfree
 */
FE_NIY void *fe_mem_stackalloc(size_t size);

/*! \brief Confident (or foolish ?) alternative to #fe_mem_stackalloc().
 *
 * Please read #fe_mem_stackalloc()'s description first.
 *
 * Basically this function
 * blindly attempts to allocate data on the stack, just like \c alloca(), and
 * doesn't check whether the allocation would cause a stack overflow.
 *
 * It is provided because quite often, that check is known to be superfluous.
 * Did a \c char[128] in a non-recursive call ever cause a stack overflow ?
 *
 * If FE_MEM_STACK_ALLOCATION_IMPLEMENTED is zero, this function performs
 * heap allocation instead.
 *
 * \see fe_mem_stackalloc
 */
FE_NIY void *fe_mem_stackalloc_confident(size_t size);

/*! \brief Atomically frees memory returned by a call to
 *         #fe_mem_stackalloc(), in case the memory was allocated on the 
 *         heap instead of the stack.
 *
 * Just like \c free(), it's a no-op if \p ptr is NULL.
 * \see fe_mem_stackalloc
 */
FE_NIY void fe_mem_stackfree(void *ptr);

/*! \brief Status codes returned by #fe_mem_stackavail(). 
 */
typedef enum {
    FE_MEM_STACKAVAIL_OK = 0, /*!<
    * You can safely use #fe_mem_stackalloc(). Go ahead ! */
    FE_MEM_STACKAVAIL_UNKNOWN, /*!<
    * The implementation can't gather enough information about the
    * stack, such that there's no way to know if a given allocation
    * will cause a stack overflow. */
    FE_MEM_STACKAVAIL_UNKNOWN_BUT_SAFE, /*!<
    * Just like the above #FE_MEM_STACKAVAIL_UNKNOWN,
    * except that safety is guaranteed by the underlying implementation
    * (as it is the case with Microsoft's \c _malloca()). */
} fe_mem_stackavail_status;

/*! \brief Atomically attempts to write the number of bytes available on the 
 *         stack to a size_t pointer.
 *         
 * #fe_mem_stackalloc() uses it implicitly to determine whether an
 * allocation should take place on the stack or on the heap.
 *
 * It is exposed in the public API more for fun than anything else.
 * You have <i>a priori</i> no reason to use
 * it, because no matter the status code or the claimed number of bytes 
 * available, #fe_mem_stackalloc() and #fe_mem_stackalloc_confident() always
 * do the job in their respective ways.
 *
 * I just figured there would be a use case for this one day.
 *
 * Trivia : This function is named after the mysterious 
 * \c stackavail() function which seemed to exist long ago.
 *
 * Implementation details :
 * - Linux: The \c getrlimit(RLIMIT_STACK) system call is used to retrieve 
 *   both the soft limit and the hard limit on the process's stack size.
 *   Fields n°28 and n°29 are read from \c /proc/self/task/<gettid()>/stat 
 *   to get, respectively, the address of the stack's bottom and the address
 *   of the stack pointer at that time for the current thread.
 *   I've noticed that stack allocations can cause segmentation faults when
 *   the number of seemingly available bytes (as naively computed by 
 *   `stack_soft_limit - (stack_bottom - stack_pointer)`) is approximately
 *   between 0 and 12288 (3*4096). I believe this behaviour is 
 *   dependent on Linux's ASLR implementation and the page size (as reported
 *   by sysconf(_SC_PAGESIZE) or sysconf(_SC_PAGE_SIZE)). Explanations are
 *   welcome.
 *   Page size on my laptop is 4096.
 *   Therefore, the \c FE_MEM_STACK_SAFETY_MARGIN_LINUX constant is defined
 *   to \c (1ul<<14ul) (that is, 4*4096) and represents exactly what the
 *   name suggests. In the future, its value might be computed at runtime
 *   instead.
 *   So, the result placed into \p available is computed like so :
 *   \code{.c}
 *       // In the future, safety_margin could be computed at runtime instead.
 *       const size_t safety_margin = FE_MEM_STACK_SAFETY_MARGIN_LINUX;
 *       #if FE_MEM_STACK_GROWTH_DIRECTION < 0
 *       const size_t naive_used = stack_bottom - stack_pointer;
 *       #elif FE_MEM_STACK_GROWTH_DIRECTION > 0
 *       const size_t naive_used = stack_pointer - stack_bottom;
 *       #else
 *       const size_t naive_used = stack_pointer > stack_bottom
 *                               ? stack_pointer - stack_bottom;
 *                               : stack_bottom - stack_pointer;
 *       #endif
 *       const size_t naive_avail = stack_soft_limit - naive_used;
 *       *available = naive_avail > safety_margin 
 *                  ? naive_avail - safety_margin 
 *                  : 0;
 *   \endcode
 *
 * \param available Pointer to where the number of bytes available on the stack
 *                  should be written.
 * \return One of the error codes listed at #fe_mem_stackavail_status.
 *         If it is not #FE_MEM_STACKAVAIL_OK (which is known to be zero),
 *         you can't rely on the value pointed to by \p available.
 * \see fe_mem_stackavail_status
 * \see fe_mem_stackalloc
 * \see fe_mem_stackalloc_confident
 * \see fe_mem_stackfree
 */
FE_NIY fe_mem_stackavail_status fe_mem_stackavail(size_t *available);

/*! @} */

#endif /* FE_MEM_STACK_H */
