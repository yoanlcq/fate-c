#include <fate/fate.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef FE_TARGET_EMSCRIPTEN
    #include <emscripten.h>
#endif

#if defined(FE_TARGET_WINDOWS)
    #include <windows.h>
#else
    #include <errno.h>
    #include <limits.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/mman.h>
    #include <sys/uio.h>
    #include <fcntl.h>
#endif

#if defined(FE_TARGET_FREEBSD)
    #include <sys/sysctl.h>
#elif defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS)
    #include <mach-o/dyld.h>
    #include <sys/param.h>
    #include <sys/mount.h>
#endif


#define TAG "fe_iov"


#if defined(FE_TARGET_LINUX) && !defined(FE_TARGET_ANDROID)
#define NUL_FILE_PATH "/dev/null"
#elif defined(FE_TARGET_WINDOWS)
#define NUL_FILE_PATH "nul"
#endif
#define NUL_FILE_FALLBACK_NAME ".nul"
static FILE *nul_file = NULL;
static char *nul_file_path = "";

static char *static_sdl_basepath = NULL;

void fe_iov_setup(void) {
    static_sdl_basepath = SDL_GetBasePath();
    if(!static_sdl_basepath)
        static_sdl_basepath = SDL_strdup("./");
#ifdef NUL_FILE_PATH
    nul_file_path = NUL_FILE_PATH;
#elif defined(FE_TARGET_ANDROID)
    static const char *nul_name = "/" NUL_FILE_FALLBACK_NAME;
    const size_t nul_name_len = strlen(nul_name);
    const char *internal = SDL_AndroidGetInternalStoragePath();
    const size_t internal_len = strlen(internal);
    nul_file_path = fe_mem_heapalloc(nul_name_len+internal_len+1, char, "");
    fe_dbg_hope(nul_file_path);
    strcpy(nul_file_path, internal);
    strcat(nul_file_path, nul_name);

    /* XXX might want to get more rigourous about this later. */
    mkdir(internal, 0755);
    /* BTW, while we're at it... */
    mkdir(SDL_AndroidGetExternalStoragePath(), 0755);
    /*
    struct stat sb;
    int status = stat(internal, &sb);
    if(status || (!status && !S_ISDIR(sb.st_mode))) {
        unlink(internal);
        ...
    }
    */
#else
    nul_file_path = fe_mem_heapalloc(strlen(NUL_FILE_FALLBACK_NAME)+1, char, "");
    fe_dbg_hope(nul_file_path);
    strcpy(nul_file_path, NUL_FILE_FALLBACK_NAME);
#endif
    fe_logi(TAG, "Opening `%s' as the null file "
                 "for fe_iov_get_vprintf_len().\n", nul_file_path);
    nul_file = fopen(nul_file_path, "wb");
    fe_dbg_hope(nul_file);
}
void fe_iov_cleanup(void) {
    SDL_free(static_sdl_basepath);
    fclose(nul_file);
#ifndef NUL_FILE_PATH
    #ifndef FE_TARGET_EMSCRIPTEN
        unlink(nul_file_path);
    #endif
    fe_mem_heapfree(nul_file_path);
#endif
}








size_t  fe_iov_get_vprintf_len(const char *fmt, va_list ap) {
    int cnt = vfprintf(nul_file, fmt, ap);
    fe_dbg_assert(cnt >= 0);
    fseek(nul_file, 0, SEEK_SET);
    return cnt;
}
size_t  fe_iov_get_printf_len(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int cnt = fe_iov_get_vprintf_len(fmt, ap);
    va_end(ap);
    return cnt;
}

void    fe_iov_deinit(fe_iov *iov) {
    fe_mem_heapfree(iov->base);
}
bool    fe_iov_resize(fe_iov *iov, size_t len) {
    iov->base = fe_mem_heaprealloc(iov->base, len+1, char, "");
    iov->base[len] = '\0';
    iov->len  = len;
    return !!(iov->base);
}
size_t     fe_iov_printf(fe_iov *iov, size_t offset, const char *fmt, ...) {
    
    va_list ap;
    
    va_start(ap, fmt);
    size_t len = fe_iov_get_vprintf_len(fmt, ap);
    va_end(ap);

    if(offset+len > iov->len)
        fe_iov_resize(iov, offset+len);
    
    va_start(ap, fmt);
    vsprintf(iov->base+offset, fmt, ap);
    va_end(ap);
    
    return len;
}

void    fe_iov_copy(fe_iov *iov, size_t offset, const fe_iov *src) {
    if(offset + src->len > iov->len)
        fe_iov_resize(iov, offset + src->len);
    memcpy(iov->base + offset, src->base, src->len);
}

















bool fe_fs_setcwd(const char *path) {
#if defined(FE_TARGET_EMSCRIPTEN)
    return false; /* No way to do this for now. */
#elif defined(FE_TARGET_WINDOWS)
    return SetCurrentDirectory(path);
#else
    return !chdir(path);
#endif
}

char * fe_fs_getcwd(void) {
#if defined(FE_TARGET_EMSCRIPTEN)
    int size = FE_FS_EMSCRIPTEN_GETCWD_MAXLEN;
    char *path = fe_mem_heapalloc(size, char, "fe_fs_getcwd()");
    if(!path)
        return NULL;
    EM_ASM({
        Module.stringToUTF8(FS.cwd(), $0, $1);
    }, path, size);
    return path;
#elif defined(FE_TARGET_WINDOWS)
    DWORD len = GetCurrentDirectoryW(0, NULL);
    WCHAR *wpath = fe_mem_heapalloc(len, WCHAR, "");
    if(!wpath)
        return NULL;
    GetCurrentDirectoryW(len, wpath);
    const char *path = fe_utf8_from_win32unicode(wpath);
    fe_mem_heapfree(wpath);
    return path;
#else
    size_t size = FE_FS_UNIX_GETCWD_CHUNKSIZE;
    char *path = fe_mem_heapalloc(size, char, "fe_fs_getcwd()");
    if(!path)
        return NULL;
    for(;;) {
        char *cwd = getcwd(path, size);
        if(cwd)
            break;
        if(errno != ERANGE) {
            fe_mem_heapfree(path);
            path = NULL;
            break;
        }
        size *= 2;
        path = fe_mem_heaprealloc(path, size, char, "fe_fs_getcwd()");
        if(!path)
            break;
    }
    return path;
#endif
}

uint64_t fe_fs_get_mtime(const fe_fpath fpath) {
#if defined(FE_TARGET_EMSCRIPTEN)
    fe_dbg_assert(fpath.type == FE_FPATH_EMSCRIPTEN_MEMFS);
    return EM_ASM_INT({
        var path = Module.UTF8ToString($0);
        return FS.stat(path).mtime;
    }, fpath.memfs.path);
#elif defined(FE_TARGET_WINDOWS)
    FILETIME ft;
    HANDLE fh;
    fh = CreateFile(fpath.path, GENERIC_READ, FILE_SHARE_READ, NULL, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(fh==INVALID_HANDLE_VALUE)
        return 0;
    GetFileTime(fh, NULL, NULL, &ft);
    CloseHandle(fh);
    return (((uint64_t)ft.dwHighDateTime)<<32ull)+(uint64_t)ft.dwLowDateTime;
#else
    struct stat st;
    if(stat(fpath.path, &st))
        return 0;
    return st.st_mtime;
#endif
}


fe_iov_status fe_fs_exists(const fe_fpath fpath) {
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;

#if defined(FE_TARGET_EMSCRIPTEN)
    /* XXX We could try investigating more about a way to perform
     * a Wget-exists on Emscripten.*/
    fe_dbg_hope(fpath.type != FE_FPATH_EMSCRIPTEN_WGET);

    if(fpath.type == FE_FPATH_EMSCRIPTEN_MEMFS) {
        status.success = true;
        status.current = FE_IOV_SUCCESS;
        status.exists  = EM_ASM_INT({
            var path = Module.UTF8ToString($0);
            return FS.exists(path);
        }, fpath.memfs.path);
        return status;
    }

    fe_dbg_assert(fpath.type == FE_FPATH_EMSCRIPTEN_IDB);
    int exists, error;
    emscripten_idb_exists(fpath.db_name, fpath.key, &exists, &error);
    fe_iov_status status = {0}; 
    status.exists = exists;
    status.success = !error;
    status.current = error ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
#elif defined(FE_TARGET_WINDOWS)
    WCHAR *fullpath_w = fe_utf8_to_win32unicode(fpath.path);
    if(!fullpath_w) {
        status.current = FE_IOV_FAILED_NO_MEM;
        return status;
    }
    status.exists = (GetFileAttributesW(fullpath_w) != INVALID_FILE_ATTRIBUTES);
    fe_mem_heapfree(fullpath_w);
    status.success = true;
    status.current = FE_IOV_SUCCESS;
#else
    status.exists = !access(fpath.path, F_OK);
    status.success = true;
    status.current = FE_IOV_SUCCESS;
#endif
    return status;
}


fe_iov_promise fe_fs_exists_async(const fe_fpath fpath) { return NULL; }



fe_iov_status  fe_fs_delete(const fe_fpath fpath) {
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
#if defined(FE_TARGET_EMSCRIPTEN)
    fe_dbg_assert(fpath.type != FE_FPATH_EMSCRIPTEN_WGET);
    if(fpath.type == FE_FPATH_EMSCRIPTEN_MEMFS) {
        /* FS.unlink() does not seem to return a value :( */
        EM_ASM_ARGS({
            var path = Module.UTF8ToString($0);
            FS.unlink(path);
        }, fpath.memfs.path); 
        status.success = true;
        status.current = FE_IOV_SUCCESS;
        return status;
    }
    int error;
    emscripten_idb_delete(fpath.db_name, fpath.key, &error);
    status.success = !error;
    status.current = error ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
#elif defined(FE_TARGET_WINDOWS)
    WCHAR *fullpath_w = fe_utf8_to_win32unicode(fpath.path);
    if(!fullpath_w) {
        status.current = FE_IOV_FAILED_NO_MEM;
        return status;
    }
    status.success = DeleteFileW(fullpath_w);
    status.current = status.success ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
    fe_mem_heapfree(fullpath_w);
#else
    status.success = !unlink(fpath.path);
    status.current = status.success ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
#endif
    return status;
}

fe_iov_promise fe_fs_delete_async(const fe_fpath fpath) { return NULL; }














fe_iov_status  fe_wget(fe_iov *iov, const char *url) {
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
#ifdef FE_TARGET_EMSCRIPTEN
    int len, error;
    void *ptr = iov->base; /* Warning for incompatible ptr types. */
    emscripten_wget_data(url, &ptr, &len, &error);
    iov->len = len;
    status.success = !error;
    status.current = error ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
    return status;
#else
    fe_tcp6 s;
    fe_ipv6_peer peer;
    fe_iov msg = FE_IOV_ZERO_INITIALIZER;
    fe_urlparts urlparts;
    if(!fe_urlparts_from_url(&urlparts, url))
        goto fail;
    if(!fe_ipv6_peer_from_tcp6_host(&peer, urlparts.host, 80)) {
        fe_urlparts_deinit(&urlparts);
        goto fail;
    }
    char *epath = fe_percent_encode(urlparts.path);
    char *ehost = fe_punycode_encode(urlparts.host);
    fe_iov_printf(&msg, 0, 
        "GET %s HTTP/1.1\r\nHost: %s\r\nAccept: application/octet-stream\r\n\r\n",
        epath, ehost
    );
    fe_mem_heapfree(epath);
    fe_mem_heapfree(ehost);
    fe_urlparts_deinit(&urlparts);

    if(!fe_tcp6_init(&s))
        goto fail;
    if(!fe_tcp6_connect(&s, &peer))
        goto fail;
    fe_tcp6_block(&s);
    fe_tcp6_send(&s, msg.base, msg.len);
    fe_iov_deinit(&msg);
    /*
     * Typical response header : 
     * Date: Wed, 21 Sep 2016 14:00:31 GMT<CRLF>
     * Server: Apache<CRLF>
     * Last-Modified: Fri, 29 Jan 2016 10:05:04 GMT<CRLF>
     * Accept-Ranges: bytes<CRLF>
     * Content-Length: 63855<CRLF>
     * Connection: close<CRLF>
     * Content-Type: application/pdf<CRLF>
     * Content-Language: fr<CRLF>
     * <CRLF>
     * <binary-data>
     */

    /* Receive header */
    size_t prev_offset=0, offset=0, chunksize=1024;
    fe_iov header = FE_IOV_ZERO_INITIALIZER;
    fe_iov_resize(&header, chunksize);
    char *bin_startptr = NULL;
    size_t bin_len = 0;
    for(;;) {
        ssize_t bytes_rcvd = fe_tcp6_recv(&s, header.base+offset, chunksize);
        if(bytes_rcvd <= 0) {
            fe_iov_deinit(&header);
            goto fail; /* Should not happen! */
        }
        char *end = strstr(header.base+prev_offset, "\r\n\r\n");
        if(end) {
            bin_startptr = end+4;
            bin_len = bytes_rcvd-(bin_startptr-header.base);
            break;
        }
        prev_offset = offset;
        offset += bytes_rcvd;
        fe_iov_resize(&header, offset+chunksize);
    }
    /* For now we only care about the Content-Length field. */
    char *ctlen = strstr(header.base, "Content-Length:");
    if(!ctlen) {
        fe_iov_deinit(&header);
        goto fail;
    }
    ctlen += strlen("Content-Length:");
    size_t total_bin_len = strtoull(ctlen, NULL, 10);
    fe_iov_resize(iov, total_bin_len);
    memcpy(iov->base, bin_startptr, bin_len);
    fe_iov_deinit(&header);
    fe_tcp6_recv(&s, iov->base+bin_len, total_bin_len-bin_len);
    fe_tcp6_deinit(&s);
    status.success = true;
fail:
    status.current = status.success ? FE_IOV_SUCCESS : FE_IOV_FAILED_UNKNOWN;
    return status;
#endif
}

fe_iov_promise fe_iov_load_wget_async(fe_iov *iov, const fe_fpath fpath) {return NULL;}




static inline fe_fd_mode fe_fd_modeflags_compile1(fe_fd_modeflags f) {
    fe_fd_mode m;
    fe_fd_modeflags_compile(&m, f);
    return m;
}


fe_iov_status  fe_fs_load(fe_iov *iov, const fe_fpath fpath) {
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
#ifdef FE_TARGET_EMSCRIPTEN
    switch(fpath.type) {
    case FE_FPATH_EMSCRIPTEN_IDB: 
        int len, error;
        void *ptr = iov->base; /* For incompatible ptr types warning. */
        emscripten_idb_load(params->idb_name, params->file_name, &ptr, &len, &error);
        iov->len = len;
        fe_iov_status status = {0};
        status.step = FE_IOV_STEP_COMPLETED;
        status.success = !error;
        status.current = error ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
        return status;
    case FE_FPATH_EMSCRIPTEN_WGET: 
        return fe_wget(iov, fpath);
    case FE_FPATH_EMSCRIPTEN_MEMFS: 
        /* Just keep going below, don't return */
        break;
    }
#endif
    fe_fd_mode mode = fe_fd_modeflags_compile1(
          FE_FD_READ
        | FE_FD_HINT_SEQUENTIAL_ACCESS
        | FE_FD_HINT_SINGLE_ACCESS
        | FE_FD_HINT_EARLY_ACCESS
    );
    fe_fd fd = fe_fd_open(fpath, mode);
    if(!fe_fd_is_valid(fd)) {
        status.current = FE_IOV_FAILED_UNKNOWN;
        return status;
    }
    size_t offset = iov->len;
    size_t len = fe_fd_size(fd);
    if(!fe_iov_grow(iov, len)) {
        status.current = FE_IOV_FAILED_NO_MEM;
        return status;
    }
    fe_fd_read(fd, iov->base+offset, len);
    fe_fd_close(fd);
    status.success = true;
    status.current = FE_IOV_SUCCESS;
    return status;
}

fe_iov_promise fe_fs_load_async(fe_iov *iov, const fe_fpath fpath) {return NULL;}





fe_iov_status  fe_fs_store(fe_iov *iov, const fe_fpath fpath) {
    fe_iov_status status = {0};
    status.step = FE_IOV_STEP_COMPLETED;
#ifdef FE_TARGET_EMSCRIPTEN
    switch(fpath.type) {
    case FE_FPATH_EMSCRIPTEN_IDB: 
        int error;
        emscripten_idb_store(fpath.db_name, fpath.key, iov->base, iov->len, &error);
        fe_iov_status status = {0};
        status.success = !error;
        status.current = error ? FE_IOV_FAILED_UNKNOWN : FE_IOV_SUCCESS;
        return status;

    case FE_FPATH_EMSCRIPTEN_WGET: 
        status.success = false;
        status.current = FE_IOV_FAILED_UNKNOWN;
        return status;
    case FE_FPATH_EMSCRIPTEN_MEMFS: 
        /* Just keep going below, don't return */
        break;
    }
#endif

    fe_fd_mode mode = fe_fd_modeflags_compile1(
          FE_FD_WRITE
        | FE_FD_HINT_SEQUENTIAL_ACCESS
        | FE_FD_HINT_SINGLE_ACCESS
        | FE_FD_HINT_EARLY_ACCESS
    );
    fe_fd fd = fe_fd_open(fpath, mode);
    if(!fe_fd_is_valid(fd)) {
        status.current = FE_IOV_FAILED_UNKNOWN;
        return status;
    }
    fe_fd_write(fd, iov->base, iov->len);
    fe_fd_close(fd);
    status.success = true;
    status.current = FE_IOV_SUCCESS;
    return status;
}

fe_iov_promise fe_fs_store_async(fe_iov *iov, const fe_fpath fpath) {return NULL;}


#ifdef FE_TARGET_ANDROID
static const char *static_modeflags_to_stdio_mode(fe_fd_modeflags f) {
    bool read = !!(f & FE_FD_READ);
    bool write = !!(f & FE_FD_WRITE);
    bool append = !!(f & FE_FD_APPEND);
    bool create = !!(f & FE_FD_CREATE);
    bool truncate = !!(f & FE_FD_TRUNCATE);
    bool nonexistent = !!(f & FE_FD_NONEXISTENT);
    if(read && !write && !append && !create && !truncate && !nonexistent)
        return "rb";
    if(read && write && !append && create && truncate)
        return "wb";
    if(!read && append && create)
        return "ab";
    if(read && append && create)
        return "a+b";
    if(read && write && !append && !create && !nonexistent)
        return "r+b";
    if(read && write && !append && create && truncate)
        return "w+b";
    fe_dbg_hope(0 && "No valid stdio mode for these flags!");
    return "";
}
#endif /* FE_TARGET_ANDROID */


void fe_fd_modeflags_validate(fe_fd_modeflags_validation *v, fe_fd_modeflags f) {
    v->valid = true;
    if(!(f & FE_FD_READWRITE))
        v->valid=false, v->e_neither_read_nor_write = true;
    if((f & FE_FD_APPEND) && !(f & FE_FD_WRITE))
        v->valid=false, v->e_append_without_write = true;
    if((f & FE_FD_TRUNCATE) && !(f & FE_FD_WRITE))
        v->valid=false, v->e_truncate_without_write = true;
    if((f & FE_FD_APPEND) && (f & FE_FD_TRUNCATE))
        v->valid=false, v->e_append_and_truncate = true;
    if((f & FE_FD_NONEXISTENT) && !(f & FE_FD_CREATE))
        v->w_nonexistent_without_create = true;
    if((f & FE_FD_TRUNCATE))
        v->w_linux_truncate_on_nonexistent = true;
    if((f & FE_FD_CREATE) && !(  (f & FE_FD_APPEND) || (f & FE_FD_TRUNCATE)  ))
        v->w_stdio_create_implies_append_or_truncate = true;
    if((f & FE_FD_APPEND))
        v->w_windows_no_actual_append_mode = true;
    if((f & FE_FD_WIN32_SHARE_WRITE)|| (f & FE_FD_WIN32_SHARE_DELETE))
        v->w_windows_filemapping_write_shared = true;
    if((f & FE_FD_HINT_EARLY_ACCESS) && (f & FE_FD_HINT_SEQUENTIAL_ACCESS))
        v->w_hint_both_random_and_sequential = true;
    if((f & FE_FD_HINT_EARLY_ACCESS) && (f & FE_FD_HINT_LATE_ACCESS))
        v->w_hint_both_early_and_late_access = true;
}

char* fe_fd_modeflags_validation_str(fe_fd_modeflags_validation status) {
    fe_iov iov = FE_IOV_ZERO_INITIALIZER;
    fe_iov_printf(&iov, iov.len, "%s%s%s%s%s%s%s%s%s%s%s",
        !status.e_neither_read_nor_write ? "" :
        "[E] File is missing read/write access flags.\n"
        "    Use FE_FD_READ, FE_FD_WRITE, or both (a.k.a FE_FD_READWRITE).\n",
        !status.e_append_without_write ? "" :
        "[E] Appending requires write access. Add FE_FD_WRITE.\n",
        !status.e_truncate_without_write ? "" :
        "[E] Truncating requires write access. Add FE_FD_WRITE.\n",
        !status.e_append_and_truncate ? "" :
        "[E] Can't truncate an append-mode file.\n"
        "    Remove either FE_FD_TRUNCATE or FE_FD_APPEND.\n",
        !status.w_nonexistent_without_create ? "" :
        "[W] Under Linux, the behaviour of O_EXCL without O_CREAT is undefined.\n"
        "    FE_FD_NONEXISTENT makes no sense without FE_FD_CREATE anyway.\n",
        !status.w_linux_truncate_on_nonexistent ? "" :
        "[W] Under Linux, the behaviour of O_TRUNC on a non-existent file is unspecified.\n"
        "    Here are your options:\n"
        "    - Pray that it works as expected;\n"
        "    - Just use fe_fd_truncate();\n"
        "    - Test the existence of the file before attempting to open it with these flags;\n"
        "    Altogether, it seems wiser to use fe_fd_truncate() instead of the FE_FD_TRUNCATE flag.\n",
        !status.w_stdio_create_implies_append_or_truncate ? "" :
        "[W] There's no stdio mode allowing file creation without either appending or truncating.\n"
        "    This matters on Android, because SDL_RWops are used, and on Emscripten because the stdio API is used.\n"
        "    Add FE_FD_APPEND or FE_FD_TRUNCATE.\n",
        !status.w_windows_no_actual_append_mode ? "" :
        "[W] Under Windows, there's no actual append mode - You have to use fe_fd_seek() to the end of file to emulate the expected behaviour.\n"
        "    Avoiding use of the append flag altogether prevents you from having to deal with this.\n",
        !status.w_windows_filemapping_write_shared ? "" : 
        "[W] MSDN discourages use of CreateFileMapping() on write-shared files.\n"
        "    If you plan to use fe_fd_mmap(), either be extra cautious or disallow "
        "file write share altogether, by removing both FE_FD_WIN32_SHARE_DELETE and FE_FD_WIN32_SHARE_WRITE.\n",
        !status.w_hint_both_random_and_sequential ? "" : 
        "[W] Both random and sequential access hints are specified, but they're conceptually mutually exclusive.\n",
        !status.w_hint_both_early_and_late_access ? "" : 
        "[W] Both early and late access hints are specified, but they're conceptually mutually exclusive.\n"
    );
    return iov.base;
}

void fe_fd_modeflags_compile(fe_fd_mode *m, fe_fd_modeflags f) {
#ifdef FE_TARGET_WINDOWS
    m->desired_access = 
        (FILE_GENERIC_READ  * !!(f & FE_FD_READ))
      | (FILE_GENERIC_WRITE * !!(f & FE_FD_WRITE));
    /* append is implied by GENERIC_WRITE. We have to seek manually 
     * to the end of file, which is a lot cleaner IMO. */
    m->share = 
        (FILE_SHARE_DELETE * !!(f & FE_FD_WIN32_SHARE_DELETE))
      | (FILE_SHARE_READ   * !!(f & FE_FD_WIN32_SHARE_READ))
      | (FILE_SHARE_WRITE  * !!(f & FE_FD_WIN32_SHARE_WRITE));
    if(f & FE_FD_CREATE) {
        if(f & FE_FD_TRUNCATE)
            m->creation_disposition = CREATE_ALWAYS;
        else {
            if(f & FE_FD_NONEXISTENT)
                m->creation_disposition = CREATE_NEW;
            else
                m->creation_disposition = OPEN_ALWAYS;
        }
    } else {
        if(f & FE_FD_TRUNCATE)
            m->creation_disposition = TRUNCATE_EXISTING;
        else
            m->creation_disposition = OPEN_EXISTING;
    }
    m->attrs_and_flags = 
         FILE_ATTRIBUTE_NORMAL
      | (FILE_FLAG_DELETE_ON_CLOSE * !!(f & FE_FD_WIN32_DELETE_ON_CLOSE))
      | (FILE_FLAG_SEQUENTIAL_SCAN * !!(f & FE_FD_HINT_SEQUENTIAL_ACCESS))
      | (FILE_FLAG_RANDOM_ACCESS   * !!(f & FE_FD_HINT_RANDOM_ACCESS))
      | (FILE_FLAG_WRITE_THROUGH   * !!(f & FE_FD_WIN32_WRITE_THROUGH));
#else
    switch(f & FE_FD_READWRITE) {
    case FE_FD_READ:      m->flags = O_RDONLY; break;
    case FE_FD_WRITE:     m->flags = O_WRONLY; break;
    case FE_FD_READWRITE: m->flags = O_RDWR;   break;
    }
    m->flags |= 
        (O_APPEND * !!(f & FE_FD_APPEND))
      | (O_CREAT  * !!(f & FE_FD_CREATE))
      | (O_EXCL   * !!(f & FE_FD_NONEXISTENT))
      | (O_TRUNC  * !!(f & FE_FD_TRUNCATE))
        ;
    m->posix_fadv_random     = !!(f & FE_FD_HINT_RANDOM_ACCESS);
    m->posix_fadv_sequential = !!(f & FE_FD_HINT_SEQUENTIAL_ACCESS);
    m->posix_fadv_noreuse    = !!(f & FE_FD_HINT_SINGLE_ACCESS);
    m->posix_fadv_willneed   = !!(f & FE_FD_HINT_EARLY_ACCESS);
    m->posix_fadv_dontneed   = !!(f & FE_FD_HINT_LATE_ACCESS);
    #ifdef FE_TARGET_ANDROID
        m->stdio_mode = static_mode_to_stdio_mode(f);
    #endif
#endif
}






#ifdef FE_TARGET_WINDOWS
static fe_fd static_fe_fd_open_win32(const fe_fpath fpath, fe_fd_mode mode) {
    WCHAR *fullpath_w = fe_utf8_to_win32unicode(fpath.path);
    if(!fullpath_w)
        return FE_FD_INVALID_FD;
    fe_fd fd = CreateFileW(fullpath_w, mode.desired_access, mode.share, 
            NULL, mode.creation_disposition, mode.attrs_and_flags, NULL);
    fe_mem_heapfree(fullpath_w);
    return fd;
}
#else
    #define HAS_POSIX_FADVISE (_XOPEN_SOURCE >= 600 || _POSIX_C_SOURCE >= 200112L)

static fe_fd static_fe_fd_open_unix(const fe_fpath fpath, fe_fd_mode mode) {
    fe_fd fd = open(fpath.path, mode.flags, 0666);
    #if HAS_POSIX_FADVISE
        if(mode.posix_fadv_random)
            posix_fadvise(fd, 0, 0, POSIX_FADV_RANDOM);
        if(mode.posix_fadv_sequential)
            posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
        if(mode.posix_fadv_noreuse)
            posix_fadvise(fd, 0, 0, POSIX_FADV_NOREUSE);
        if(mode.posix_fadv_willneed)
            posix_fadvise(fd, 0, 0, POSIX_FADV_WILLNEED);
        if(mode.posix_fadv_dontneed)
            posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED);
    #endif
    return fd;
}
#endif


#ifdef FE_TARGET_ANDROID

static void static_androidsdlrwops_setcallbacks(SDL_rwops *rw);

static fe_fd static_fe_fd_open_android(fe_fpath fpath, fe_fd_mode mode) {
    fe_fd fd = SDL_RWFromFile(fpath.path, mode.stdio_mode);
    if(fd)
        return fd;
    int unix_fd = static_fe_fd_open_unix(fpath, mode);
    if(unix_fd < 0)
        return FE_FD_INVALID_FD;
    fd = SDL_AllocRW();
    fd->type  = SDL_RWOPS_UNKNOWN;
    fd->hidden.unknown.data1 = (void*)(uintptr_t) unix_fd;
    #define static_androidsdlrwops_has_unix_fd(fd) ((fd)->type == SDL_RWOPS_UNKNOWN)
    #define static_androidsdlrwops_get_unix_fd(fd) ((unsigned)(uintptr_t)(fd)->hidden.unkown.data1)
    fd->hidden.unknown.data2 = NULL;
    static_androidsdlrwops_setcallbacks(fd);
}

static Sint64 SDLCALL static_androidsdlrwops_size(struct SDL_RWops * context) {
    Sint64 cur = lseek(static_androidsdlrwops_get_unix_fd(context), 0, SEEK_CUR);
    Sint64 end = lseek(static_androidsdlrwops_get_unix_fd(context), 0, SEEK_END);
    lseek(static_androidsdlrwops_get_unix_fd(context), cur, SEEK_SET);
    return end;
}

static Sint64 SDLCALL static_androidsdlrwops_seek(struct SDL_RWops * context, Sint64 offset,
                             int whence) {
    return lseek(static_androidsdlrwops_get_unix_fd(context), offset, whence);
}

static size_t SDLCALL static_androidsdlrwops_read(struct SDL_RWops * context, void *ptr,
                             size_t size, size_t maxnum) {
    ssize_t bytes_read = read(static_androidsdlrwops_get_unix_fd(context), ptr, size*maxnum);
    return bytes_read<=0 ? 0 : bytes_read/size;
}

static size_t SDLCALL static_androidsdlrwops_write(struct SDL_RWops * context, const void *ptr,
                          size_t size, size_t num) {
    ssize_t bytes_written = write(static_androidsdlrwops_get_unix_fd(context), ptr, size*num);
    return bytes_written<=0 ? 0 : bytes_written/size;
}

static int SDLCALL static_androidsdlrwops_close(struct SDL_RWops * context) {
    int ret = close(static_androidsdlrwops_get_unix_fd(context));
    SDL_FreeRW(context);
    return ret;
}

static void static_androidsdlrwops_setcallbacks(SDL_rwops *fd) {
    fd->size  = static_androidsdlrwops_size;
    fd->seek  = static_androidsdlrwops_seek;
    fd->read  = static_androidsdlrwops_read;
    fd->write = static_androidsdlrwops_write;
    fd->close = static_androidsdlrwops_close;
}

#endif

typedef struct {
    enum {
        STATIC_EMSCRIPTEN_MEMFS=0,
        STATIC_EMSCRIPTEN_IDB,
        STATIC_EMSCRIPTEN_WGET
    } type;
    struct { FILE *fp; } memfs;
    struct { fe_iov iov; } idb;
    struct { fe_iov iov; } wget;
} static_emscripten_fd;


fe_fd          fe_fd_open(const fe_fpath fpath, fe_fd_mode mode) {
    fe_fd fd = FE_FD_INVALID_FD;
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    fd = static_fe_fd_open_win32(fpath, mode);
#elif defined(FE_TARGET_ANDROID)
    fd = static_fe_fd_open_android(fpath, mode);
#else
    fd = static_fe_fd_open_unix(fpath, mode);
#endif
    return fd;
}








/* Windows: len can be 0 (to go as far as possible) 
 * for CreateFileMapping() and also for MapViewOfFile(). 
 * But Unix mmap() does not accept this. */
bool   fe_fd_mmap(fe_filemapview *v, fe_fd fd, fe_fd_offset offset, size_t len, bool rw) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    fe_dbg_assert(fe_fd_size(fd) > 0);
    v->filemapping = CreateFileMapping(fd, NULL, 
            /* There are a lot of other nice-looking flags that we should inspect. */
            rw ? PAGE_READWRITE : PAGE_READONLY,
            sizeof(len)>4 ? len>>32 : 0, len,
            NULL /* Optional name for the system-wide file mapping object */
    );
    if(!v->filemapping)
        return false; /* Info in GetLastError(). */
    static DWORD dwSysGran = 0;
    if(!dwSysGran) {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        dwSysGran = si.dwAllocationGranularity;
    }
    uint64_t fileMapStart = (offset / dwSysGran) * dwSysGran;
    DWORD dwMapViewSize = (offset % dwSysGran) + len;
    void *iViewDelta = offset - dwFileMapStart;
    v->view_base = MapViewOfFile(fmap, 
            FILE_MAP_READ | (rw*FILE_MAP_WRITE),
            fileMapStart>>32, fileMapStart, dwMapViewSize
    );
    if(!v->view_base)
        return false;
    v->base = v->view_base + iViewDelta;
    v->len = len;
    return true;
#else
    #if defined(FE_TARGET_ANDROID)
        if(!static_androidsdlrwops_has_unix_fd(fd))
            return false; /* No point in mmap()ing a compressed file anyway. */
    #endif
    static int pagesize = 0;
    if(!pagesize)
        pagesize = sysconf(_SC_PAGE_SIZE);
    uint64_t start = (offset / pagesize) * pagesize;
    size_t viewsize = (offset % pagesize) + len;
    size_t viewdelta = offset - start;

    v->view_base = mmap(NULL, viewsize, PROT_READ | (rw*PROT_WRITE), MAP_PRIVATE, 
    #if defined(FE_TARGET_ANDROID)
            static_androidsdlrwops_get_unix_fd(fd),
    #else
            fd, 
    #endif
            start);
    if(v->view_base == MAP_FAILED)
        return false;
    v->base = v->view_base + viewdelta;
    v->len = len;
    return true;
#endif
}

bool fe_fd_msync_hint(fe_filemapview *v) {
    size_t total_len = (v->base - v->view_base) + v->len;
#ifdef FE_TARGET_WINDOWS
    return FlushViewOfFile(v->view_base, total_len);
#else
    return !msync(v->view_base, total_len, MS_ASYNC);
#endif
}
void fe_fd_munmap(fe_filemapview *v) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    UnmapViewOfFile(v->view_base);
    CloseHandle(v->filemapping);
#else
    size_t total_len = (v->base - v->view_base) + v->len;
    /* There's no guarantee that the content is flushed otherwise. */
    msync(v->view_base, total_len, MS_SYNC | MS_INVALIDATE);
    munmap(v->view_base, total_len);
#endif
}


fe_fd_offset   fe_fd_seek(fe_fd fd, fe_fd_offset offset, fe_fd_seek_whence whence) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    LARGE_INTEGER req_offset, new_offset;
    req_offset.QuadPart = offset;
    /* BOOL success = */SetFilePointerEx(fd, req_offset, &new_offset, whence);
    /* fe_dbg_hope(success); //XXX */
    return new_offset.QuadPart;
#elif defined(FE_TARGET_ANDROID)
    return SDL_RWSeek(fd, offset, whence);
#else
    return lseek(fd, offset, whence);
#endif
}

fe_fd_offset fe_fd_size(fe_fd fd) {
#ifdef FE_TARGET_WINDOWS
    LARGE_INTEGER li = {{0}};
    /* BOOL success = */GetFileSizeEx(fd, &li);
    /* fe_dbg_hope(sucess); //XXX */
    return li.QuadPart;
#else
    fe_fd_offset cur = fe_fd_seek(fd, 0, FE_FD_SEEK_CUR);
    fe_fd_offset end = fe_fd_seek(fd, 0, FE_FD_SEEK_END);
    fe_fd_seek(fd, cur, FE_FD_SEEK_SET);
    return end;
#endif
}

ssize_t        fe_fd_read(fe_fd fd, void *buf, size_t len) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    DWORD bytes_read;
    BOOL success = ReadFile(fd, buf, len, &bytes_read, NULL);
    return success ? bytes_read : -1;
#elif defined(FE_TARGET_ANDROID)
    return SDL_RWread(fd, buf, 1, len);
#else
    return read(fd, buf, len);
#endif
}
ssize_t        fe_fd_write(fe_fd fd, const void *buf, size_t len) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    DWORD bytes_written;
    BOOL success = WriteFile(fd, buf, len, &bytes_written, NULL);
    return success ? bytes_read : -1;
#elif defined(FE_TARGET_ANDROID)
    return SDL_RWwrite(fd, buf, 1, len);
#else
    return write(fd, buf, len);
#endif
}
ssize_t        fe_fd_readv(fe_fd fd, fe_iov *iov_array, size_t iov_count) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID)
    /* ReadFileScatter() doesn't do what we want. It can only fill 
     * page-sized buffers, and it is asynchronous. */
    ssize_t bytes_read;
    size_t i;
    for(bytes_read=i=0 ; i<iov_count ; ++i)
        bytes_read += fe_fd_read(fd, iov_array[i].base, iov_array[i].len);
    return bytes_read;
#else
    return readv(fd, (struct iovec*)iov_array, iov_count);
#endif
}
ssize_t        fe_fd_writev(fe_fd fd, const fe_iov *iov_array, size_t iov_count) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID)
    ssize_t bytes_written;
    size_t i;
    for(bytes_written=i=0 ; i<iov_count ; ++i)
        bytes_written += fe_fd_write(fd, iov_array[i].base, iov_array[i].len);
    return bytes_written;
#else
    return writev(fd, (struct iovec*)iov_array, iov_count);
#endif
}
ssize_t        fe_fd_preadv(fe_fd fd, fe_iov *iov_array, size_t iov_count, fe_fd_offset offset) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID) \
   || defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS)
    fe_fd_offset old = fe_fd_seek(fd, 0, FE_FD_SEEK_CUR);
    fe_fd_seek(fd, offset, FE_FD_SEEK_SET);
    ssize_t bytes_read = fe_fd_readv(fd, iov_array, iov_count);
    fe_fd_seek(fd, old, FE_FD_SEEK_SET);
    return bytes_read;
#else
    return preadv(fd, (struct iovec*)iov_array, iov_count, offset);
#endif
}
ssize_t        fe_fd_pwritev(fe_fd fd, const fe_iov *iov_array, size_t iov_count, fe_fd_offset offset) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID) \
   || defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS)
    fe_fd_offset old = fe_fd_seek(fd, 0, FE_FD_SEEK_CUR);
    fe_fd_seek(fd, offset, FE_FD_SEEK_SET);
    ssize_t bytes_written = fe_fd_writev(fd, iov_array, iov_count);
    fe_fd_seek(fd, old, FE_FD_SEEK_SET);
    return bytes_written;
#else
    return pwritev(fd, (struct iovec*)iov_array, iov_count, offset);
#endif
}
bool           fe_fd_sync(fe_fd fd) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    return FlushFileBuffers(fd);
#elif defined(FE_TARGET_ANDROID)
    if(!static_androidsdlrwops_has_unix_fd(fd))
        return false;
    return !fsync(static_androidsdlrwops_get_unix_fd(fd));
#else
    return !fsync(fd);
#endif
}
bool           fe_fd_truncate(fe_fd fd, size_t len) {
#ifdef FE_TARGET_EMSCRIPTEN
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    fe_fd_offset cur = fe_fd_seek(fd, 0, FE_FD_SEEK_CUR);
    fe_fd_seek(fd, len, FE_FD_SEEK_SET);
    SetEndOfFile(fd);
    fe_fd_seek(fd, 0, cur);
#elif defined(FE_TARGET_ANDROID)
    if(!static_androidsdlrwops_has_unix_fd(fd))
        return false;
    return !ftruncate(static_androidsdlrwops_get_unix_fd(fd), len);
#else
    return !ftruncate(fd, len);
#endif
}
void           fe_fd_close(fe_fd fd) {
#ifdef FE_TARGET_EMSCRIPTEN
    /* XXX Should Log a warning if fe_fd_sync() was not called. */
    fe_dbg_hope(0 && "This is not implemented yet !");
#elif defined(FE_TARGET_WINDOWS)
    CloseHandle(fd);
#elif defined(FE_TARGET_ANDROID)
    SDL_RWclose(fd);
#else
    if(!close(fd))
        return;
    switch(errno) {
    case EINTR: fe_loge(TAG, "close() failed with %s !\n", "EINTR"); break;
    case EIO:   fe_loge(TAG, "close() failed with %s !\n", "EIO");   break;
    }
#endif
}

bool fe_iov_promise_poll(fe_iov_promise p, fe_iov_progress *st) {return 0;}
bool fe_iov_promise_wait(fe_iov_promise p, fe_iov_progress *st, int timeout_milliseconds) {return 0;}
void fe_iov_promise_cancel(fe_iov_promise p) {}









/*
 *
 *
 *
 *
 *
 *
 *
 *    fe_fs_get_executable_dir()
 *
 *
 *
 *
 *
 *
 *
 */


#if defined(FE_TARGET_EMSCRIPTEN)
char *fe_fs_get_executable_dir(void) {
    return ""; /* FIXME */
}
#elif defined(FE_TARGET_LINUX)

#ifndef FE_TARGET_ANDROID
#if !(BSD_SOURCE || _XOPEN_SOURCE >= 500  \
 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED || _POSIX_C_SOURCE >= 200112L)
#ifdef _GNU_SOURCE
#include <sys/syscall.h>
static inline ssize_t readlink(const char *path, char *buf, size_t bufsiz) {
    return syscall(SYS_readlink, path, buf, bufsiz);
}
static inline int lstat(const char *path, struct stat *buf) {
    return syscall(SYS_lstat, path, buf);
}
#else
#error Syscalls lstat() and readlink() are not available. Please update (g)libc or define _GNU_SOURCE.
#endif
#endif
#endif /* !FE_TARGET_ANDROID */

#if __GLIBC__ > 2 || (__GLIBC__==2 && __GLIBC_MINOR__>=16)
#include <sys/auxv.h>
#endif
static char *get_executable_path(void) {
    struct stat st;
    char *str2, *str;
#if __GLIBC__ > 2 || (__GLIBC__==2 && __GLIBC_MINOR__>=16)
    str = (char*)getauxval(AT_EXECFN);
    if(str)
        return realpath(str, NULL);
#endif
    if(lstat("/proc/self/exe", &st) == 0) {
        str = fe_mem_heapalloc(st.st_size+1, char, "");
        if(readlink("/proc/self/exe", str, st.st_size) > 0) {
            str[st.st_size] = '\0';
            str2 = realpath(str, NULL);
            fe_mem_heapfree(str);
            return str2;
        }
        fe_mem_heapfree(str);
    }
    return NULL;
}

#elif defined(FE_TARGET_FREEBSD)

static char *get_executable_path(void) {
    static char appdir[PATH_MAX];
    char procfs[2][19] = {
        "/proc/curproc/exe",
        "/proc/curproc/file"
    };
    int i, mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
    size_t size = PATH_MAX;
    struct stat st;
    char *str, *str2;
    for(i=0 ; i<2 ; ++i) {
        if(lstat(procfs[i], &st) == 0) {
            str = fe_mem_heapalloc(st.st_size+1, char, "");
            if(readlink(procfs[i], str, st.st_size) > 0) {
                str[st.st_size] = '\0';
                str2 = realpath(str, NULL);
                fe_mem_heapfree(str);
                return str2;
            }
            fe_mem_heapfree(str);
        }
    }
    if(sysctl(mib, 4, appdir, &size, NULL, 0) == 0)
        return realpath(appdir, NULL);
    return NULL;
}


#elif defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS)

static char *get_executable_path(void) {
    static char appdir[MAXPATHLEN];
    char *str, *str2;
    uint32_t size = MAXPATHLEN;
    if(_NSGetExecutablePath(appdir, &size) == 0)
        return realpath(appdir, NULL);
    str = fe_mem_heapalloc(size, char, "");
    _NSGetExecutablePath(str, &size);
    str2 = realpath(str, NULL);
    fe_mem_heapfree(str);
    return str2;
}

#elif defined(FE_TARGET_WINDOWS)
static char *get_executable_path(void) {
    return strdup(_pgmptr);
}
#endif

#ifndef FE_TARGET_EMSCRIPTEN

#if defined(FE_TARGET_WINDOWS)
#define PATHSEP "\\"
#else
#define PATHSEP "/"
#endif

static bool remove_last_path_component(char *path) {
    char *ind = strrchr(path, *PATHSEP);
    if(*ind) {
        *ind = '\0';
        return true;
    }
    return false;
}

char *fe_fs_get_executable_dir(void) {
    char *expath = get_executable_path();
    if(!expath)
        return NULL;
    size_t size = strlen(expath);
    char *res  = fe_mem_heapalloc(size+strlen(PATHSEP"res")+1, char, "");
    char *data = fe_mem_heapalloc(size+strlen(PATHSEP"data")+1, char, "");

    int i;
    for(i=0 ; i<3 ; ++i) {
        if(!remove_last_path_component(expath))
            break;
        strcpy(res,  expath);
        strcat(res,  PATHSEP"res");
        strcpy(data, expath);
        strcat(data, PATHSEP"data");
        /* FIXME!!
        if(fe_fs_exists(res) && fe_fs_exists(data)) {
            fe_mem_heapfree(res);
            fe_mem_heapfree(data);
            return expath;
        }
        */
    }
    fe_mem_heapfree(res);
    fe_mem_heapfree(data);
    fe_loge(TAG, "Could not find res/ and data/ directories.\n");
    return NULL;
}

#undef PATHSEP
#endif /* EMSCRIPTEN */




