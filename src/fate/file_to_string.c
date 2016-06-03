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

/*! \file fate/file_to_string.c
 *  \brief File to string.
 *
 * TODO
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fate/defs.h>
#include <fate/log.h>
#include <fate/fatal_alloc.h>
#include <fate/file_to_string.h>

char **fe_file_to_string_array(FILE *file, size_t *num_strings) {

    /* The weirdest thing in history. Without the following line, 
     * subsequent fread() calls return -1 with EINVAL if the file had been
     * opened and closed (!) before in the program's lifetime.
     * Very probably a MinGW issue. */
    /* strerror(errno); */

    long oldpos = ftell(file);

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    *num_strings = 0;

    char **strings = fe_fatal_malloc(((filesize/BUFSIZ)+1), sizeof(char*));
    long i; /* Keep it signed. */
    size_t bytes_read;
    for(i=0 ; i<filesize/BUFSIZ ; ++i) {
        strings[i] = fe_fatal_malloc(BUFSIZ+1, 1);
        bytes_read = fread(strings[i], 1, BUFSIZ, file);
        if(bytes_read <= 0) {
            /* Something's not right if we're here. */
            fclose(file);
            for(--i ; i>=0 ; --i)
                free(strings[i]);
            free(strings);
            *num_strings = 0;
            return NULL;
        }
        strings[i][BUFSIZ] = '\0';
        ++(*num_strings);
    }

    strings[i] = fe_fatal_malloc(filesize-(i*BUFSIZ)+1, 1);
    bytes_read = fread(strings[i], 1, BUFSIZ, file);
    strings[i][bytes_read] = '\0';
    ++(*num_strings);

    fseek(file, oldpos, SEEK_SET);

    return strings;
}

char *fe_file_to_string(FILE *file, uint64_t *length) {

    long oldpos = ftell(file);
    char *buf, tmp_buf[BUFSIZ];
    size_t bytes_read;
    uint64_t buf_len;

    rewind(file);
    for(buf=NULL, buf_len=0 ; ; ) {
        bytes_read = fread(tmp_buf, 1, BUFSIZ, file);
        if(bytes_read <= 0)
            break;
        buf = fe_fatal_realloc(buf, buf_len+bytes_read+1, 1);
        memcpy(buf+buf_len, tmp_buf, bytes_read);
        buf_len += bytes_read;
    }
    buf[buf_len] = '\0';

    fseek(file, oldpos, SEEK_SET);

    if(length)
        *length = buf_len;

    return buf;
}
