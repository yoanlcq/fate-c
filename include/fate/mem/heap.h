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
 *  \ingroup mem
 *
 * @{
 */


#ifndef FE_MEM_HEAP_H
#define FE_MEM_HEAP_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fate/defs.h>
#include <fate/mt.h>

/*! \brief Info about a piece of memory allocated by #fe_mem_heapalloc() and 
 *         friends. */
typedef struct {
    void *base;  /*!< Base address to allocated memory within this block. */
    size_t memb_size; /*!< Size of each member. */
    size_t nmemb; /*!< Number of members. */
    const char *type_str; /*!< Member type as a string, e.g "int". */
    const char *tag; /*!< Tag given at allocation time. */
    const char *filename; /*!< The name of the source file in which the 
                               allocation took place. */
    size_t lineno; /*!< The line in the source file, at which the 
                          allocation took place. */
} fe_mem_heap_block;

/*! \brief Lock this mutex for as long as you use #fe_mem_heap_blocks. */
extern fe_mt_mutex fe_mem_heap_blocks_mutex;
/*! \brief Global access to metadata about heap-allocated pieces of memory. 
 *
 * The pointer is not const because one may want to edit data it points to.
 * Don't see a use case for this, though.
 */
extern fe_mem_heap_block *fe_mem_heap_blocks;
/*! \brief Number of members held by ~fe_mem_heap_blocks. */
extern size_t fe_mem_heap_block_count;

#include <fate/log.h>
static inline void fe_mem_handle_failure(const char *funcname, size_t nmemb, 
                                  const char *type_str, const char *tag) {
    fe_fatal("fe_mem", "%s() failed: "
             "Could not allocate space for %"PRIsize_t" '%s's (tag:'%s').\n", 
             funcname, nmemb, type_str, tag);
}


#if __DOXYGEN__ || defined(FE_MEM_DBG)

FE_DECL_NIY void *fe_mem_heapalloc_real(size_t nmemb, size_t size, const char *tag, 
                    const char *type_str, 
                    const char *filename, unsigned lineno) 
                    FE_DECL_MALLOC;
FE_DECL_NIY void *fe_mem_xheapalloc_real(size_t nmemb, size_t size, const char *tag, 
                    const char *type_str, 
                    const char *filename, unsigned lineno) 
                    FE_DECL_MALLOC;
/*! \brief TODO */
#define fe_mem_heapalloc(nmemb, type, tag) \
        fe_mem_heapalloc_real(nmemb, sizeof(type), tag, #type, __FILE__, __LINE__)
/*! \brief TODO */
#define fe_mem_xheapalloc(nmemb, type, tag) \
        fe_mem_xheapalloc_real(nmemb, sizeof(type), tag, #type, __FILE__, __LINE__)


FE_DECL_NIY void *fe_mem_heapcalloc_real(size_t nmemb, size_t size, const char *tag, 
                         const char *type_str,
                         const char *filename, unsigned lineno) 
                         FE_DECL_MALLOC;
FE_DECL_NIY void *fe_mem_xheapcalloc_real(size_t nmemb, size_t size, const char *tag, 
                         const char *type_str,
                         const char *filename, unsigned lineno) 
                         FE_DECL_MALLOC;
/*! \brief TODO */
#define fe_mem_heapcalloc(nmemb, type, tag) \
        fe_mem_heapcalloc_real(nmemb, sizeof(type), tag, #type, __FILE__, __LINE__)
/*! \brief TODO */
#define fe_mem_xheapcalloc(nmemb, type, tag) \
        fe_mem_xheapcalloc_real(nmemb, sizeof(type), tag, #type, __FILE__, __LINE__)

FE_DECL_NIY void *fe_mem_heaprealloc_real(void *ptr, 
                          size_t nmemb, size_t size, const char *tag, 
                          const char *type_str,
                          const char *filename, unsigned lineno);
FE_DECL_NIY void *fe_mem_xheaprealloc_real(void *ptr, 
                          size_t nmemb, size_t size, const char *tag, 
                          const char *type_str,
                          const char *filename, unsigned lineno);
/*! \brief TODO */
#define fe_mem_heaprealloc(ptr, nmemb, type, tag) \
  fe_mem_heaprealloc_real(ptr, nmemb, sizeof(type), tag, #type, __FILE__, __LINE__)
/*! \brief TODO */
#define fe_mem_xheaprealloc(ptr, nmemb, type, tag) \
  fe_mem_xheaprealloc_real(ptr, nmemb, sizeof(type), tag, #type, __FILE__, __LINE__)


/*! \brief TODO */
FE_DECL_NIY void fe_mem_heapfree(void *ptr);

/*! \brief TODO */
FE_DECL_NIY void fe_mem_heaplock(void);

/*! \brief TODO */
FE_DECL_NIY void fe_mem_heapunlock(void);

/*! \brief Get blocks info.
 *
 * The caller <b>must</b> wrap their code in a 
 * #fe_mem_heaplock()/#fe_mem_heapunlock() scope 
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
FE_DECL_NIY size_t fe_mem_heap_get_block_count(void);
/*! \brief Get blocks info.
 *
 * The caller <b>must</b> wrap their code in a 
 * #fe_mem_heaplock()/#fe_mem_heapunlock() scope 
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
FE_DECL_NIY fe_mem_heapblockinfo* fe_mem_heap_get_blocks_info(void);

/*! \brief Get a block index, given an arbitrary address. 
 *
 * \param addr An arbitrary address.
 * \param index If \p addr is within a block's allocated memory, the 
 *              block's index is stored here.
 *              It may then be used for calls to #fe_mem_getblocksinfo().
 * \return Is \p addr indeed within a block ? 
 * \see fe_mem_getblocksinfo */
FE_DECL_NIY size_t fe_mem_get_block_index(void *addr);

/*! \brief Atomically simulates limits on the memory allocated 
 *         by #fe_mem_heapalloc() and friends.
 *
 * Limits are disabled by default.
 *
 * When profiling, #fe_mem_heapalloc() and friends check if they can allocate
 * memory without breaking those virtual limits. If not, they fail by
 * returning NULL.
 *
 * The point is to see how the engine behaves in
 * limited memory environnements, and thus test its robustness.
 *
 * \param total_nbytes Set it to 0 to remove this limit. 
 *           Limits how many bytes, in total, can be allocated via
 *           #fe_mem_heapalloc and friends.
 * \param heap_size Set it to 0 to remove this limit. 
 *           Limits the imaginary heap's size in bytes. 
 *           Therefore, it takes memory fragmentation into account.
 * \return 1 If the \p total_nbytes limit is already broken,
 *         2 If the \p heap_size limit is already broken,
 *         3 If both limits are already broken, and
 *         0 Otherwise.
 * \see fe_mem_getlimits
 */
FE_DECL_NIY size_t fe_mem_limits(size_t total_nbytes, size_t heap_size);

/*! \brief Atomically gets the values set by the last call to 
 *         #fe_mem_limits().
 *
 * \see fe_mem_limits
 */
FE_DECL_NIY void fe_mem_getlimits(size_t *total_nbytes, size_t *heap_size);

#else /* ifdef FE_MEM_DBG */

#include <stdlib.h>

#define fe_mem_heaplock() 
#define fe_mem_heapunlock() 
#define fe_mem_heapalloc(nmemb, type, tag) malloc((nmemb)*sizeof(type))
static inline void* fe_mem_xheapalloc_real(size_t size, size_t nmemb, const char *type_str, const char *tag) {
    void *res = malloc(nmemb*size);
    if(!res)
        fe_mem_handle_failure("xm", nmemb, type_str, tag);
    return res;
}
#define fe_mem_xheapalloc(nmemb, type, tag) \
        fe_mem_xheapalloc_real(sizeof(type), nmemb, #type, tag)
#define fe_mem_heapcalloc(nmemb, type, tag) calloc(nmemb, sizeof(type))
static inline void* fe_mem_xheapcalloc_real(size_t size, size_t nmemb, const char *type_str, const char *tag) {
    void *res = calloc(nmemb, size);
    if(!res)
        fe_mem_handle_failure("xc", nmemb, type_str, tag);
    return res;
}
#define fe_mem_xheapcalloc(nmemb, type, tag) \
        fe_mem_xheapcalloc_real(sizeof(type), nmemb, #type, tag)
#define fe_mem_heaprealloc(ptr, nmemb, type, tag) \
            realloc(ptr, (nmemb)*sizeof(type))
static inline void* fe_mem_xheaprealloc_real(size_t size, void *ptr, size_t nmemb, const char *type_str, const char *tag) {
    void *res = realloc(ptr, nmemb*size);
    if(!res)
        fe_mem_handle_failure("xre", nmemb, type_str, tag);
    return res;
}
#define fe_mem_xheaprealloc(ptr, nmemb, type, tag) \
        fe_mem_xheaprealloc_real(sizeof(type), ptr, nmemb, #type, tag)
#define fe_mem_heapfree(ptr) free(ptr)
#define fe_mem_getblocksinfo(index, nblocks, blocks) 0
#define fe_mem_getblockindex(addr, index) false
#define fe_mem_limits(total_nbytes, heap_size)
#define fe_mem_getlimits(total_nbytes, heap_size) \
            (*(total_nbytes)=*(heap_size)=0)

#endif /* ifdef FE_MEM_DBG */

/*! @} */

#endif /* FE_MEM_HEAP_H */
