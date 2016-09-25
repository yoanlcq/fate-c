#include <fate/fate.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

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

void fe_iov_setup(void) {

#ifdef FE_TARGET_EMSCRIPTEN
    {
        struct stat sb;
        int status = stat("/memfs", &sb);
        fe_dbg_hope(status && S_ISDIR(sb.st_mode));
        mkdir("/wget", 0755);
        mkdir("/idb", 0755);
    }
#endif

#ifdef NUL_FILE_PATH
    nul_file_path = NUL_FILE_PATH;
#elif defined(FE_TARGET_ANDROID)
    nul_file_path = fe_asprintf("/%s/%s", 
            SDL_AndroidGetExternalStoragePath(), 
            NUL_FILE_FALLBACK_NAME
    );
    fe_dbg_hope(nul_file_path);
    /* XXX might want to get more rigourous about this later. */
    mkdir(SDL_AndroidGetInternalStoragePath(), 0755);
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
    nul_file_path = fe_asprintf("%s", NUL_FILE_FALLBACK_NAME);
    fe_dbg_hope(nul_file_path);
#endif
    fe_logi(TAG, "Opening `%s' as the null file "
                 "for fe_iov_get_vprintf_len().\n", nul_file_path);
    nul_file = fopen(nul_file_path, "wb");
    fe_dbg_hope(nul_file);
}
void fe_iov_cleanup(void) {
    fclose(nul_file);
#ifndef NUL_FILE_PATH
    unlink(nul_file_path);
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
    size_t cnt = fe_iov_get_vprintf_len(fmt, ap);
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
size_t     fe_iov_vprintf(fe_iov *iov, size_t offset, const char *fmt, va_list ap) {
    
    va_list ap2;
    va_copy(ap2, ap);
    size_t len = fe_iov_get_vprintf_len(fmt, ap2);
    va_end(ap2);

    if(offset+len > iov->len)
        fe_iov_resize(iov, offset+len);
    
    vsprintf(iov->base+offset, fmt, ap);
    
    return len;
}
size_t     fe_iov_printf(fe_iov *iov, size_t offset, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    size_t len = fe_iov_vprintf(iov, offset, fmt, ap);
    va_end(ap);
    return len;
}

void    fe_iov_copy(fe_iov *iov, size_t offset, const fe_iov *src) {
    if(offset + src->len > iov->len)
        fe_iov_resize(iov, offset + src->len);
    memcpy(iov->base + offset, src->base, src->len);
}










#ifdef FE_TARGET_WINDOWS
static char* static_strerror(DWORD error) {
    LPWSTR lpMsgBuf;

    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        /*MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),*/
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (void*)&lpMsgBuf,
        0, NULL);

    char *utf8 = fe_utf8_from_win32unicode(lpMsgBuf);
    LocalFree(lpMsgBuf);
    return utf8;
}
#elif defined(FE_TARGET_EMSCRIPTEN) || defined(FE_TARGET_ANDROID)
static char* static_strerror(int err) {
    return fe_asprintf("%s", strerror(err));
}
#else
#include <locale.h>
static char* static_strerror(int err) {
    /* Replace "C" by "" to generate a locale suitable for the user. 
     * XXX We should allocate it only once. */
    locale_t lc = newlocale(LC_ALL_MASK, "C", 0);
    char *str = fe_asprintf("%s", strerror_l(err, lc));
    freelocale(lc);
    return str;
}
#endif


/* XXX unfinished. */
char *fe_iov_status_str(fe_iov_status status) {
    /*
    switch(status.current) {
    case FE_IOV_SUCCESS: break;
    case FE_IOV_FAILED_SYSERR: 
        return static_strerror(status.last_error);
    }
    */
    return fe_asprintf("<TODO>");
}




bool fe_fs_setcwd(const char *path) {
#if defined(FE_TARGET_WINDOWS)
    return SetCurrentDirectory(path);
#else
    return !chdir(path);
#endif
}

char * fe_fs_getcwd(void) {
#if defined(FE_TARGET_WINDOWS)
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
#if defined(FE_TARGET_WINDOWS)
    FILETIME ft;
    HANDLE fh;
    fh = CreateFile(fpath.path, GENERIC_READ, FILE_SHARE_READ, NULL, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(fh==INVALID_HANDLE_VALUE)
        return 0;
    GetFileTime(fh, NULL, NULL, &ft);
    CloseHandle(fh);
    return (((uint64_t)ft.dwHighDateTime)<<32ull)+(uint64_t)ft.dwLowDateTime;
#elif defined(FE_TARGET_EMSCRIPTEN)
    if(fpath.is_wget)
        return 0;
    char *path;
    if(fpath.is_memfs)
        path = fe_asprintf("%s", fpath.memfs.path);
    else
        path = fe_asprintf("idb/%s/%s", fpath.idb.db_name, fpath.idb.path);
    struct stat st;
    uint64_t ret = 0;
    if(!stat(path, &st))
        ret = st.st_mtime;
    fe_mem_heapfree(path);
    return ret;
#else
    struct stat st;
    if(stat(fpath.path, &st))
        return 0;
    return st.st_mtime;
#endif
}


bool fe_fs_exists(const fe_fpath fpath) {
#if defined(FE_TARGET_EMSCRIPTEN)
    /* XXX We could try investigating more about a way to perform
     * a Wget-exists on Emscripten.*/
    if(fpath.is_wget)
        return false;

    if(fpath.is_memfs)
        return !access(fpath.memfs.path, F_OK);

    fe_dbg_assert(fpath.is_idb);
    int exists, error;
    emscripten_idb_exists(fpath.idb.db_name, fpath.idb.path, &exists, &error);
    return !error * exists;
#elif defined(FE_TARGET_WINDOWS)
    WCHAR *fullpath_w = fe_utf8_to_win32unicode(fpath.path);
    if(!fullpath_w)
        return false;
    bool exists = (GetFileAttributesW(fullpath_w) != INVALID_FILE_ATTRIBUTES);
    fe_mem_heapfree(fullpath_w);
    return exists;
#else
    return !access(fpath.path, F_OK);
#endif
}


fe_iov_promise fe_fs_exists_async(const fe_fpath fpath) { return NULL; }



bool  fe_fs_delete(const fe_fpath fpath) {
#if defined(FE_TARGET_EMSCRIPTEN)
    fe_dbg_assert(!fpath.is_wget);
    if(fpath.is_memfs)
        return !unlink(fpath.memfs.path);
    int error;
    emscripten_idb_delete(fpath.idb.db_name, fpath.idb.path, &error);
    return !error;
#elif defined(FE_TARGET_WINDOWS)
    WCHAR *fullpath_w = fe_utf8_to_win32unicode(fpath.path);
    if(!fullpath_w)
        return false;
    bool success = DeleteFileW(fullpath_w);
    fe_mem_heapfree(fullpath_w);
    return success;
#else
    return !unlink(fpath.path);
#endif
}

fe_iov_promise fe_fs_delete_async(const fe_fpath fpath) { return NULL; }














bool fe_wget(fe_iov *iov, const char *url) {
#ifdef FE_TARGET_EMSCRIPTEN
    int len, error;
    void *mem; /* Warning for incompatible ptr types. */
    emscripten_wget_data(url, &mem, &len, &error);
    iov->base = mem;
    iov->len = len;
    return !error;
#else
    fe_tcp6 s;
    fe_ipv6_peer peer;
    fe_urlparts urlparts;
    if(!fe_urlparts_from_url(&urlparts, url))
        return false;
    if(!fe_ipv6_peer_from_tcp6_host(&peer, urlparts.host, 80)) {
        fe_urlparts_deinit(&urlparts);
        return false;
    }
    char *epath = fe_percent_encode(urlparts.path);
    char *ehost = fe_punycode_encode(urlparts.host);
    fe_iov msg = {0};
    fe_iov_printf(&msg, 0, 
        "GET %s HTTP/1.1\r\nHost: %s\r\nAccept: application/octet-stream\r\n\r\n",
        epath, ehost
    );
    fe_mem_heapfree(epath);
    fe_mem_heapfree(ehost);
    fe_urlparts_deinit(&urlparts);

    if(!fe_tcp6_init(&s))
        return false;
    if(!fe_tcp6_connect(&s, &peer))
        return false;
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
    fe_iov header = {0};
    fe_iov_resize(&header, chunksize);
    char *bin_startptr = NULL;
    size_t bin_len = 0;
    for(;;) {
        ssize_t bytes_rcvd = fe_tcp6_recv(&s, header.base+offset, chunksize);
        if(bytes_rcvd <= 0) {
            fe_iov_deinit(&header);
            return false; /* Should not happen! */
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
        return false;
    }
    ctlen += strlen("Content-Length:");
    size_t total_bin_len = strtoull(ctlen, NULL, 10);
    fe_iov_resize(iov, total_bin_len);
    memcpy(iov->base, bin_startptr, bin_len);
    fe_iov_deinit(&header);
    fe_tcp6_recv(&s, iov->base+bin_len, total_bin_len-bin_len);
    fe_tcp6_deinit(&s);
    return true;
#endif
}

fe_iov_promise fe_iov_load_wget_async(fe_iov *iov, const fe_fpath fpath) {return NULL;}




static inline fe_fd_mode fe_fd_modeflags_compile1(fe_fd_modeflags f) {
    fe_fd_mode m;
    fe_fd_modeflags_compile(&m, f);
    return m;
}


bool fe_fs_load(fe_iov *iov, const fe_fpath fpath) {
#ifdef FE_TARGET_EMSCRIPTEN
    if(fpath.is_wget)
        return fe_wget(iov, fpath.wget.url);
    if(fpath.is_idb) {
        int len, error;
        void *ptr = iov->base; /* For incompatible ptr types warning. */
        emscripten_idb_load(fpath.idb.db_name, fpath.idb.path, &ptr, &len, &error);
        iov->len = len;
        return !error;
    }
    /* If MEMFS, just go through. */
#endif
    fe_fd_mode mode = fe_fd_modeflags_compile1(
          FE_FD_READ
        | FE_FD_HINT_SEQUENTIAL_ACCESS
        | FE_FD_HINT_SINGLE_ACCESS
        | FE_FD_HINT_EARLY_ACCESS
    );
    fe_fd fd = fe_fd_open(fpath, mode);
    if(!fe_fd_is_valid(fd))
        return false;
    size_t offset = iov->len;
    size_t len = fe_fd_size(fd);
    if(!fe_iov_grow(iov, len))
        return false;
    fe_fd_read(fd, iov->base+offset, len);
    fe_fd_close(fd);
    return true;
}

fe_iov_promise fe_fs_load_async(fe_iov *iov, const fe_fpath fpath) {return NULL;}





bool fe_fs_store(fe_iov *iov, const fe_fpath fpath) {
#ifdef FE_TARGET_EMSCRIPTEN
    if(fpath.is_wget)
        return false;
    if(fpath.is_idb) {
        int error;
        emscripten_idb_store(fpath.idb.db_name, fpath.idb.path, iov->base, iov->len, &error);
        return !error;
    }
    /* Otherwise, just go through */
#endif

    fe_fd_mode mode = fe_fd_modeflags_compile1(
          FE_FD_WRITE
        | FE_FD_HINT_SEQUENTIAL_ACCESS
        | FE_FD_HINT_SINGLE_ACCESS
        | FE_FD_HINT_EARLY_ACCESS
    );
    fe_fd fd = fe_fd_open(fpath, mode);
    if(!fe_fd_is_valid(fd))
        return false;
    fe_fd_write(fd, iov->base, iov->len);
    fe_fd_close(fd);
    return true;
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
    fe_iov iov = {0};
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
        "    This matters on Android, because SDL_RWops are used.\n"
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
        m->stdio_mode = static_modeflags_to_stdio_mode(f);
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

static int static_fe_fd_open_unix(const char *path, fe_fd_mode mode) {
    int fd = open(path, mode.flags, 0666);
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

static void static_androidsdlrwops_setcallbacks(SDL_RWops *rw);

static fe_fd static_fe_fd_open_android(fe_fpath fpath, fe_fd_mode mode) {
    fe_fd fd = SDL_RWFromFile(fpath.path, mode.stdio_mode);
    if(fd)
        return fd;
    int unix_fd = static_fe_fd_open_unix(fpath.path, mode);
    if(unix_fd < 0)
        return FE_FD_INVALID_FD;
    fd = SDL_AllocRW();
    fd->type  = SDL_RWOPS_UNKNOWN;
    fd->hidden.unknown.data1 = (void*)(intptr_t) unix_fd;
    #define static_androidsdlrwops_has_unix_fd(fd) ((fd)->type == SDL_RWOPS_UNKNOWN)
    #define static_androidsdlrwops_get_unix_fd(fd) ((int)(intptr_t)(fd)->hidden.unknown.data1)
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

static void static_androidsdlrwops_setcallbacks(SDL_RWops *fd) {
    fd->size  = static_androidsdlrwops_size;
    fd->seek  = static_androidsdlrwops_seek;
    fd->read  = static_androidsdlrwops_read;
    fd->write = static_androidsdlrwops_write;
    fd->close = static_androidsdlrwops_close;
}

#endif


fe_fd          fe_fd_open(const fe_fpath fpath, fe_fd_mode mode) {
    fe_fd fd = FE_FD_INVALID_FD;
#ifdef FE_TARGET_EMSCRIPTEN
    if(fpath.is_memfs) {
        fd.unix_fd = static_fe_fd_open_unix(fpath.memfs.path, mode);
    } else if(fpath.is_wget) {
        fe_fpath memfs_fpath = {{{0}}};
        memfs_fpath.is_memfs = true;
        /* TODO would be better not having to use a hash function. */
        memfs_fpath.memfs.path = fe_asprintf("wget/%"PRIx64"", fe_hash_sdbm(fpath.wget.url, ~(size_t)0));
        if(!fe_fs_exists(memfs_fpath)) {
            fe_iov iov = {0};
            fe_wget(&iov, fpath.wget.url);
            fe_fs_store(&iov, memfs_fpath);
            fe_iov_deinit(&iov);
        }
        fd.unix_fd = static_fe_fd_open_unix(memfs_fpath.memfs.path, mode);
        fe_mem_heapfree(memfs_fpath.memfs.path);
    } else {
        fe_fpath memfs_fpath = {{{0}}};
        memfs_fpath.is_memfs = true;
        memfs_fpath.memfs.path = fe_asprintf("idb/%s/%s", fpath.idb.db_name, fpath.idb.path);
        if(!fe_fs_exists(memfs_fpath)) {
            if(!(mode.flags & O_CREAT)) {
                fe_mem_heapfree(memfs_fpath.memfs.path);
                return fd;
            }
            if(!(mode.flags & O_TRUNC)) {
                fe_iov iov = {0};
                fe_fs_load(&iov, fpath);
                fe_fs_store(&iov, memfs_fpath);
                fe_iov_deinit(&iov);
            }
        }
        fd.unix_fd = static_fe_fd_open_unix(memfs_fpath.memfs.path, mode);
        fe_mem_heapfree(memfs_fpath.memfs.path);
        fd.idb.path    = fe_asprintf("%s", fpath.idb.path);
        fd.idb.db_name = fe_asprintf("%s", fpath.idb.db_name);
    }
    fd.is_idb = fpath.is_idb;
    fd.is_readonly = !((mode.flags & O_WRONLY) 
                   || ((mode.flags & O_RDWR) == O_RDWR) 
                   || (mode.flags & O_APPEND));
#elif defined(FE_TARGET_WINDOWS)
    fd = static_fe_fd_open_win32(fpath, mode);
#elif defined(FE_TARGET_ANDROID)
    fd = static_fe_fd_open_android(fpath, mode);
#else
    fd = static_fe_fd_open_unix(fpath.path, mode);
#endif
    return fd;
}






static inline bool static_unix_mmap(fe_filemapview *v, int fd, fe_fd_offset offset, size_t len, bool rw) {
    static int pagesize = 0;
    if(!pagesize)
        pagesize = sysconf(_SC_PAGESIZE);
    uint64_t start = (offset / pagesize) * pagesize;
    size_t viewsize = (offset % pagesize) + len;
    size_t viewdelta = offset - start;

    v->view_base = mmap(NULL, viewsize, PROT_READ | (rw*PROT_WRITE), MAP_PRIVATE, fd, start);
    if(v->view_base == MAP_FAILED)
        return false;
    v->base = v->view_base + viewdelta;
    v->len = len;
    return true;
}



/* Windows: len can be 0 (to go as far as possible) 
 * for CreateFileMapping() and also for MapViewOfFile(). 
 * But Unix mmap() does not accept this. */
bool   fe_fd_mmap(fe_filemapview *v, fe_fd fd, fe_fd_offset offset, size_t len, bool rw) {
#if defined(FE_TARGET_WINDOWS)
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
    return static_unix_mmap(v, 
    #ifdef FE_TARGET_EMSCRIPTEN
            fd.unix_fd,
    #elif defined(FE_TARGET_ANDROID)
            static_androidsdlrwops_get_unix_fd(fd),
    #else
            fd, 
    #endif
    offset, len, rw);
#endif
}

bool fe_fd_msync_hint(fe_filemapview *v) {
    size_t total_len = (v->base - v->view_base) + v->len;
#if defined(FE_TARGET_WINDOWS)
    return FlushViewOfFile(v->view_base, total_len);
#else
    return !msync(v->view_base, total_len, MS_ASYNC);
#endif
}

bool fe_fd_munmap(fe_filemapview *v) {
#if defined(FE_TARGET_WINDOWS)
    UnmapViewOfFile(v->view_base);
    CloseHandle(v->filemapping);
    return true;
#else
    size_t total_len = (v->base - v->view_base) + v->len;
    /* There's no guarantee that the content is flushed otherwise. */
    msync(v->view_base, total_len, MS_SYNC | MS_INVALIDATE);
    return !munmap(v->view_base, total_len);
#endif
}


fe_fd_offset   fe_fd_seek(fe_fd fd, fe_fd_offset offset, fe_fd_seek_whence whence) {
#if defined(FE_TARGET_WINDOWS)
    LARGE_INTEGER req_offset, new_offset;
    req_offset.QuadPart = offset;
    BOOL success = SetFilePointerEx(fd, req_offset, &new_offset, whence);
    return success ? new_offset.QuadPart : -1;
#elif defined(FE_TARGET_ANDROID)
    return SDL_RWseek(fd, offset, whence);
#elif defined(FE_TARGET_EMSCRIPTEN)
    return lseek(fd.unix_fd, offset, whence);
#else
    return lseek(fd, offset, whence);
#endif
}

fe_fd_offset fe_fd_size(fe_fd fd) {
#ifdef FE_TARGET_WINDOWS
    LARGE_INTEGER li;
    BOOL success = GetFileSizeEx(fd, &li);
    return success ? li.QuadPart : -1;
#else
    fe_fd_offset cur = fe_fd_seek(fd, 0, FE_FD_SEEK_CUR);
    fe_fd_offset end = fe_fd_seek(fd, 0, FE_FD_SEEK_END);
    fe_fd_seek(fd, cur, FE_FD_SEEK_SET);
    return end;
#endif
}

ssize_t        fe_fd_read(fe_fd fd, void *buf, size_t len) {
#if defined(FE_TARGET_WINDOWS)
    DWORD bytes_read;
    BOOL success = ReadFile(fd, buf, len, &bytes_read, NULL);
    return success ? bytes_read : -1;
#elif defined(FE_TARGET_ANDROID)
    return SDL_RWread(fd, buf, 1, len);
#elif defined(FE_TARGET_EMSCRIPTEN)
    return read(fd.unix_fd, buf, len);
#else
    return read(fd, buf, len);
#endif
}

ssize_t        fe_fd_write(fe_fd fd, const void *buf, size_t len) {
#if defined(FE_TARGET_WINDOWS)
    DWORD bytes_written;
    BOOL success = WriteFile(fd, buf, len, &bytes_written, NULL);
    return success ? bytes_read : -1;
#elif defined(FE_TARGET_ANDROID)
    return SDL_RWwrite(fd, buf, 1, len);
#elif defined(FE_TARGET_EMSCRIPTEN)
    return write(fd.unix_fd, buf, len);
#else
    return write(fd, buf, len);
#endif
}

ssize_t        fe_fd_readv(fe_fd fd, fe_iov *iov_array, size_t iov_count) {
#if defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID)
    /* ReadFileScatter() doesn't do what we want. It can only fill 
     * page-sized buffers, and it is asynchronous. */
    ssize_t bytes_read;
    size_t i;
    for(bytes_read=i=0 ; i<iov_count ; ++i)
        bytes_read += fe_fd_read(fd, iov_array[i].base, iov_array[i].len);
    return bytes_read;
#else
    FE_COMPILETIME_ASSERT(sizeof(iov_array->base) == sizeof(((struct iovec){0}).iov_len), "");
    FE_COMPILETIME_ASSERT(offsetof(fe_iov,len) == offsetof(struct iovec,iov_len), "");
    FE_COMPILETIME_ASSERT(sizeof(fe_iov) == sizeof(struct iovec), "");
    #ifdef FE_TARGET_EMSCRIPTEN
        return readv(fd.unix_fd, (struct iovec*)iov_array, iov_count);
    #else
        return readv(fd, (struct iovec*)iov_array, iov_count);
    #endif
#endif
}

ssize_t        fe_fd_writev(fe_fd fd, const fe_iov *iov_array, size_t iov_count) {
#if defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID)
    ssize_t bytes_written;
    size_t i;
    for(bytes_written=i=0 ; i<iov_count ; ++i)
        bytes_written += fe_fd_write(fd, iov_array[i].base, iov_array[i].len);
    return bytes_written;
#elif defined(FE_TARGET_EMSCRIPTEN)
    return writev(fd.unix_fd, (struct iovec*)iov_array, iov_count);
#else
    return writev(fd, (struct iovec*)iov_array, iov_count);
#endif
}

ssize_t        fe_fd_preadv(fe_fd fd, fe_iov *iov_array, size_t iov_count, fe_fd_offset offset) {
#if defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID) \
 || defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS) \
 || defined(FE_TARGET_EMSCRIPTEN)
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
#if defined(FE_TARGET_WINDOWS) || defined(FE_TARGET_ANDROID) \
 || defined(FE_TARGET_OSX) || defined(FE_TARGET_IOS) \
 || defined(FE_TARGET_EMSCRIPTEN)
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
    if(!(fd.is_idb && !fd.is_readonly))
        return true;
    fe_filemapview v = {0};
    fe_fd_mmap(&v, fd, 0, fe_fd_size(fd), false);
    fe_iov iov = {v.base, v.len};
    fe_fpath fpath = {{{0}}};
    fpath.is_idb = true;
    fpath.idb = fd.idb;
    fe_fs_store(&iov, fpath);
    fe_fd_munmap(&v);
    return true;
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
#if defined(FE_TARGET_WINDOWS)
    fe_fd_offset cur = fe_fd_seek(fd, 0, FE_FD_SEEK_CUR);
    fe_fd_seek(fd, len, FE_FD_SEEK_SET);
    SetEndOfFile(fd);
    fe_fd_seek(fd, 0, cur);
#elif defined(FE_TARGET_ANDROID)
    if(!static_androidsdlrwops_has_unix_fd(fd))
        return false;
    return !ftruncate(static_androidsdlrwops_get_unix_fd(fd), len);
#elif defined(FE_TARGET_EMSCRIPTEN)
    return !ftruncate(fd.unix_fd, len);
#else
    return !ftruncate(fd, len);
#endif
}

bool           fe_fd_close(fe_fd fd) {
#ifdef FE_TARGET_EMSCRIPTEN
    if(fd.is_idb) {
        fe_fd_sync(fd);
        fe_mem_heapfree(fd.idb.path);
        fe_mem_heapfree(fd.idb.db_name);
    }
    return !close(fd.unix_fd);
#elif defined(FE_TARGET_WINDOWS)
    return CloseHandle(fd);
#elif defined(FE_TARGET_ANDROID)
    return !SDL_RWclose(fd);
#else
    return !close(fd);
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




