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

#include <utils/pathto.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if defined(__cplusplus)
#define maybe_cast(__t__, __o__) static_cast<__t__>(__o__)
#else
#define maybe_cast(__t__, __o__) (__o__)
#endif


#if defined(__linux__) 

#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/auxv.h>
#include <unistd.h>

char *get_executable_path(void) {
    struct stat st;
    char *str = (char*)getauxval(AT_EXECFN), *str2;
    if(str)
        return realpath(str, NULL);
    if(lstat("/proc/self/exe", &st) == 0) {
        str = maybe_cast(char*, malloc(st.st_size+1));
        if(readlink("/proc/self/exe", str, st.st_size) > 0) {
            str[st.st_size] = '\0';
            str2 = realpath(str, NULL);
            free(str);
            return str2;
        }
        free(str);
    }
    return NULL;
}


#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)

#include <limits.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>

char *get_executable_path(void) {
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
            str = maybe_cast(char*, malloc(st.st_size+1));
            if(readlink(procfs[i], str, st.st_size) > 0) {
                str[st.st_size] = '\0';
                str2 = realpath(str, NULL);
                free(str);
                return str2;
            }
            free(str);
        }
    }
    if(sysctl(mib, 4, appdir, &size, NULL, 0) == 0)
        return realpath(appdir, NULL);
    return NULL;
}


#elif defined(__APPLE__) && defined(__MACH__)

#include <limits.h>
#include <mach-o/dyld.h>

char *get_executable_path(void) {
    static char appdir[MAXPATHLEN];
    char *str, *str2;
    size_t size = MAXPATHLEN;
    if(_NSGetExecutablePath(appdir, &size) == 0)
        return realpath(appdir, NULL);
    str = maybe_cast(char*, malloc(size))
    _NSGetExecutablePath(str, &size);
    str2 = realpath(str, NULL);
    free(str);
    return str2;
}

#elif defined(_WIN32)
#include <windows.h>
char *get_executable_path(void) {
    size_t size = strlen(_pgmptr);
    char *ind, *res = maybe_cast(char*, malloc(size+1));
    strcpy(res, _pgmptr);
    for(;;) {
        ind = strchr(res, '\\');
        if(ind == NULL)
            break;
        *ind = '/';
    }
    return res;
}
#endif


char *pathparent(int n, char *path) {
    char *ind;
    for( ; n != 0 ; --n) {
        ind = strrchr(path, '/');
        if(ind == NULL)
            break;
        *ind = '\0';
    }
    return path;
}

char *uptoappdir(char *expath) {
    char *res, *data;
    size_t size = strlen(expath);
    int i;

    res  = maybe_cast(char*, malloc(size+5));
    data = maybe_cast(char*, malloc(size+6));

    for(i=0 ; i<3 ; ++i) {
        pathparent(1, expath);
        strcpy(res,  expath);
        strcat(res,  "/res");
        strcpy(data, expath);
        strcat(data, "/data");
        if(exists(res) && exists(data)) {
            free(res);
            free(data);
            return expath;
        }
    }
    free(res);
    free(data);
    fprintf(stderr, "Could not find res/ and data/ directories.\n");
    exit(EXIT_FAILURE);
}


char *appdir;
size_t appdir_len = 0;

#define cbs PATHTO_CIRCULAR_BUFFER_SIZE
#define cbi pathto_circular_buffer_index
#define cbb pathto_circular_buffer

char *cbb[cbs];
int cbi = 0;
char *pathto(char *path) {
    char **str;

    if(!appdir_len) {
        appdir = uptoappdir(get_executable_path());
        appdir_len = strlen(appdir);
    }

    if(path[0] == '@') {
        str = &cbb[cbi];
        if(*str != NULL)
            free(*str);
        *str = maybe_cast(char*, malloc(appdir_len+strlen(path)));
        strcpy(*str, appdir);
        strcat(*str, path+1);
        ++cbi;
        cbi %= cbs;
        return *str;
    }

    return path;
}
#undef cbb
#undef cbs
#undef cbi

/* Test section */
/*
#include <stdio.h>
int main(void) { 
    char *path = pathto("@");
    puts(path ? path : "Nope, it didn't work.\n");
    return 0;
}
*/

