#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fate/defs.h>
#include <fate/log.h>
#include <fate/fatal_alloc.h>
#include <fate/file_to_string.h>

char **fate_file_to_string_array(FILE *file, size_t *num_strings) {

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

    char **strings = fate_fatal_malloc(((filesize/BUFSIZ)+1), sizeof(char*));
    long i; /* Keep it signed. */
    size_t bytes_read;
    for(i=0 ; i<filesize/BUFSIZ ; ++i) {
        strings[i] = fate_fatal_malloc(BUFSIZ+1, 1);
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

    strings[i] = fate_fatal_malloc(filesize-(i*BUFSIZ)+1, 1);
    bytes_read = fread(strings[i], 1, BUFSIZ, file);
    strings[i][bytes_read] = '\0';
    ++(*num_strings);

    fseek(file, oldpos, SEEK_SET);

    return strings;
}

char *fate_file_to_string(FILE *file, uint64_t *length) {

    long oldpos = ftell(file);
    char *buf, tmp_buf[BUFSIZ];
    size_t bytes_read;
    uint64_t buf_len;

    rewind(file);
    for(buf=NULL, buf_len=0 ; ; ) {
        bytes_read = fread(tmp_buf, 1, BUFSIZ, file);
        if(bytes_read <= 0)
            break;
        buf = fate_fatal_realloc(buf, buf_len+bytes_read+1, 1);
        memcpy(buf+buf_len, tmp_buf, bytes_read);
        buf_len += bytes_read;
    }
    buf[buf_len] = '\0';

    fseek(file, oldpos, SEEK_SET);

    if(length)
        *length = buf_len;

    return buf;
}
