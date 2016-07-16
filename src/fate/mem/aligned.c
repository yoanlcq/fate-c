
/* This is placeholder code, that will maybe be used later. */

/*
void* fe_mem_heapalloc_aligned(size_t size, size_t alignment) {
#if defined(FE_ASM_HAS_MM_MALLOC)
    return _mm_malloc(size, alignment);
#elif defined(_MSC_VER)
    return _aligned_malloc(size, alignment);
#elif defined(FE_C11_SUPPORT)
    // Alignment must be a power of two, and size must be a multiple of alignment.
    return aligned_alloc(alignment, size);
#elif defined(HAS_POSIX_MEMALIGN)
    // Alignment must be a power of two and a multiple of sizeof(void *).
    void *mem;
    int err = posix_memalign(&mem, alignment, size);
    // handle err==EINVAL or err==ENOMEM. No, this is not errno.
    return err ? NULL : mem;
#endif
}

void fe_mem_aligned_heapfree(void *mem) {
#if defined(FE_ASM_HAS_MM_MALLOC)
    _mm_free(mem);
#elif defined(_MSC_VER)
    _aligned_free(mem);
#else
    free(mem);
#endif
}

*/
