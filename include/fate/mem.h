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

/*! \file fate/mem.h
 *  \brief Debug malloc() wrappers, heap inspection, and more.
 *  \defgroup mem Mem : Debug malloc() wrappers, heap inspection, and more.
 *
 * <b>Compile-time switches :</b>
 * <table>
 * <tr><td>FE_MEM_DEBUG</td><td>If not defined, heap allocator 
 *  macros expand to regular stdlib calls, and debug/profiling-related
 *  functionalities are compiled out, simply replaced by assignment 
 *  operators when is makes sense.</td></tr>
 * </table>
 * 
 * @{
 */


#ifndef FE_MEM_H
#define FE_MEM_H

#include <stddef.h>
#include <stdbool.h>
#include <fate/defs.h>

/*! \brief TODO */
void fe_mem_setup(void);

/*! \brief Heap block info, as stored by calls to #fe_mem_malloc() and 
 *         friends. */
struct fe_mem_blockinfo {
    void *start;  /*!< Base address to allocated memory within this block. */
    size_t memb_size; /*!< Size of each member. */
    size_t nmemb; /*!< Number of members. */
    const char *type_str; /*!< Member type as a string, e.g "int". */
    const char *tag; /*!< Tag given at allocation time. */
    const char *filename; /*!< The name of the source file in which the 
                               allocation took place. */
    unsigned lineno; /*!< The line in the source file, at which the 
                          allocation took place. */
};
/*! \brief TODO */
typedef struct fe_mem_blockinfo fe_mem_blockinfo;


#if __DOXYGEN__ || defined(FE_MEM_DEBUG)

/*! \brief TODO */
#ifdef __GNUC__
__attribute__((malloc))
#endif
void *fe_mem_malloc_real(size_t nmemb, size_t size, const char *tag, 
                           const char *type_str, 
                           const char *filename, unsigned lineno);
/*! \brief TODO */
#define fe_mem_malloc(nmemb, type, tag) \
            fe_mem_lock(); \
            fe_mem_malloc_real(nmemb, sizeof(type), \
                                 tag, #type, __FILE__, __LINE__); \
            fe_mem_unlock();

/*! \brief TODO */
#ifdef __GNUC__
__attribute__((malloc))
#endif
void *fe_mem_calloc_real(size_t nmemb, size_t size, const char *tag, 
                           const char *type_str,
                           const char *filename, unsigned lineno);
/*! \brief TODO */
#define fe_mem_calloc(nmemb, type, tag) \
            fe_mem_lock(); \
            fe_mem_calloc_real(nmemb, sizeof(type), \
                                 tag, #type, __FILE__, __LINE__); \
            fe_mem_unlock();

/*! \brief TODO */
void *fe_mem_realloc_real(void *ptr, 
                            size_t nmemb, size_t size, const char *tag, 
                            const char *type_str,
                            const char *filename, unsigned lineno);
/*! \brief TODO */
#define fe_mem_realloc(ptr, nmemb, type, tag) \
            fe_mem_lock(); \
            fe_mem_realloc_real(ptr, nmemb, sizeof(type), \
                                  tag, #type, __FILE__, __LINE__); \
            fe_mem_unlock();

/*! \brief TODO */
void fe_mem_free_real(void *ptr);
/*! \brief TODO */
#define fe_mem_free(ptr) \
            fe_mem_lock(); \
            fe_mem_free_real(ptr); \
            fe_mem_unlock(); \

/*! \brief TODO */
void fe_mem_lock(void);

/*! \brief TODO */
void fe_mem_unlock(void);

/*! \brief Get blocks info.
 *
 * The caller <b>must</b> wrap their code in a 
 * #fe_mem_lock()/#fe_mem_unlock() scope 
 * for as long as they intend to use the returned blocks, in order to prevent 
 * other threads from allocating/freeing blocks, thus potentially invalidating 
 * them.
 *
 * \param index Requested block index, starting at 0.
 * \param nblocks Requested number of blocks.
 * \param blocks An array where to store the requested blocks. It should be
 *               large enough to hold at least \p nblocks #fe_mem_blockinfo 
 *               records.
 * \return The actual number of blocks stored in \p blocks, never greater than
 *         \p nblocks.
 * \see fe_mem_getblockindex
 */
unsigned long fe_mem_getblocksinfo(unsigned long index, 
                                     unsigned long nblocks,          
                                     fe_mem_blockinfo *blocks);

/*! \brief Get a block index, given an arbitrary address. 
 *
 * \param addr An arbitrary address.
 * \param index If \p addr is within a block's allocated memory, the 
 *              block's index is stored here.
 *              It may then be used for calls to #fe_mem_getblocksinfo().
 * \return Is \p addr indeed within a block ? 
 * \see fe_mem_getblocksinfo */
bool fe_mem_getblockindex(void *addr, unsigned long *index);

/*! \brief Atomically simulates limits on the memory allocated 
 *         by #fe_mem_malloc() and friends.
 *
 * Limits are disabled by default.
 *
 * When profiling, #fe_mem_malloc() and friends check if they can allocate
 * memory without breaking those virtual limits. If not, they fail by
 * returning NULL.
 *
 * The point is to see how the engine behaves in
 * limited memory environnements, and thus test its robustness.
 *
 * \param total_nbytes Set it to 0 to remove this limit. 
 *           Limits how many bytes, in total, can be allocated via
 *           #fe_mem_malloc and friends.
 * \param heap_size Set it to 0 to remove this limit. 
 *           Limits the imaginary heap's size in bytes. 
 *           Therefore, it takes memory fragmentation into account.
 * \return 1 If the \p total_nbytes limit is already broken,
 *         2 If the \p heap_size limit is already broken,
 *         3 If both limits are already broken, and
 *         0 Otherwise.
 * \see fe_mem_getlimits
 */
unsigned fe_mem_limits(unsigned long total_nbytes, unsigned long heap_size);

/*! \brief Atomically gets the values set by the last call to 
 *         #fe_mem_limits().
 *
 * \see fe_mem_limits
 */
void fe_mem_getlimits(unsigned long *total_nbytes, unsigned long *heap_size);

#else /* ifdef FE_MEM_DEBUG */

#include <stdlib.h>
#define fe_mem_lock() 
#define fe_mem_unlock() 
#define fe_mem_malloc(nmemb, type, tag) malloc((nmemb)*sizeof(type))
#define fe_mem_calloc(nmemb, type, tag) calloc(nmemb, sizeof(type))
#define fe_mem_realloc(ptr, nmemb, type, tag) \
            realloc(ptr, (nmemb)*sizeof(type))
#define fe_mem_free(ptr) free(ptr)
#define fe_mem_getblocksinfo(index, nblocks, blocks) 0
#define fe_mem_getblockindex(addr, index) false
#define fe_mem_limits(total_nbytes, heap_size)
#define fe_mem_getlimits(total_nbytes, heap_size) \
            (*(total_nbytes)=*(heap_size)=0)

#endif /* ifdef FE_MEM_DEBUG */

/*! \brief Atomically allocates memory on the stack, optionally getting how 
 *         many bytes were available.
 *         <b>Please read the description</b>.
 * 
 * Here is what you should know about stack allocation :
 * - It's crazy fast, compared to heap allocation (with \c malloc());
 * - Since it's not on the heap, it can help avoiding memory fragmentation in
 *   multithreaded programs;
 * - Stack size is limited by the OS, so it should never be used to allocate
 *   big chunks of data;
 * - On some platforms, stack size can be grown on-demand;
 * - Data is automatically freed as soon as the function you are in returns.
 *   <b>But see below</b>.
 *
 * This function attempts to allocate \p size bytes on the stack, but falls
 * back to a regular call to \c malloc() when one of the following 
 * conditions are met :
 * - No stack allocator (such as \c alloca() or \c _malloca()) is provided 
 *   for the current compiler or platform;
 * - There is no way to get the process' stack size and stack pointer;
 * - The above conditions are not met, and it is known that the allocation
 *   would cause a stack overflow.
 *
 * #fe_mem_stackfree() is provided because of such potential fallbacks.<br>
 * The caller must use it when they are done with the allocated memory.<br>
 * It will be a no-op if the memory has indeed been allocated from the 
 * stack, or a call to \c free() otherwise.
 *
 * <b>Implementation detail</b> : If required, #fe_mem_stackalloc() may 
 * allocate an <b>extra byte</b> right before the returned address.
 * The said byte would be used internally by #fe_mem_stackfree() so it 
 * can take the appropriate decision.<br>
 * Please take this into account when estimating whether there is enough stack 
 * space left, but <b>never</b> rely on its actual presence.
 *
 * Bad usage example :
 * \code{.c}
 * #include <stddef.h> //For NULL
 * #include <fate/log.h>
 * #include <fate/mem.h>
 *
 * static const char *TAG = "Bad example";
 * #define LEN 4
 * int *stored;
 * 
 * void fill(void) {
 *     unsigned long i;
 *     stored = fe_mem_stackalloc(LEN*sizeof(*stored), NULL);
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
 * void display(void) {
 *     unsigned long i;
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
 *     int *values = fe_mem_stackalloc(2*sizeof(int), NULL);
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
 * \param left <b>Optional</b> output parameter indicating how many bytes were 
 *             available on the stack <b>before</b> allocation, or one of the
 *             following error codes :<br>
 *             -1 if there is no underlying stack allocator available;<br>
 *             -2 if a stack allocator is available, but the stack size 
 *             and/or stack pointer cannot be determined.<br>
 *             In both cases you can be certain that #fe_mem_stackalloc() will
 *             fall back to \c malloc().
 * \return Pointer to \p size uninitialized bytes that may be used to store 
 *         data within the current function's scope, or NULL on failure.
 * \see fe_mem_stackfree
 */
void *fe_mem_stackalloc(size_t size, size_t *left);

/*! \brief Atomically frees memory returned by a call to
 *         #fe_mem_stackalloc(), in case the memory was allocated on the 
 *         heap instead of the stack.
 *
 * Just like \c free(), it's a no-op if \p ptr is NULL.
 * \see fe_mem_stackalloc
 */
void fe_mem_stackfree(void *ptr);

/*! @} */

#endif /* FE_MEM_H */
