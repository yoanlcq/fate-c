#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <opengl_base.h>
#include <glmake.h>

struct glmake_shaders_db  glmake_shaders_db  = {NULL, 0};
struct glmake_programs_db glmake_programs_db = {NULL, 0};

/* 
 * This hash function is sbdm, from http://www.cse.yorku.ca/~oz/hash.html
 * It is public domain.
 */
uint64_t glmake_hashfunc(const char *str)
{
    int c;
    uint64_t hash = 0;
    while (c = *str++)
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
}


#if defined(__WIN32)
#include <windows.h>
uint64_t glmake_last_write_time(const char *path) {
    FILETIME ft;
    HANDLE fh;
    fh = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    GetFileTime(fh, NULL, NULL, &ft);
    CloseHandle(fh);
    return (((uint64_t)ft.dwHighDateTime)<<32)+ft.dwLowDateTime;
}
#else
#include <sys/stat.h>
#include <unistd.h>
uint64_t glmake_last_write_time(const char *path) {
    struct stat st;
    stat(path, &st);
    return st.st_mtime;
}
#endif



struct glmake_shaders_db_entry *find_shaders_hashtable_entry(uint64_t hash)
{
    int i;
    if(glmake_shaders_db.entries != NULL)
        for(i=glmake_shaders_db.top-1 ; i>=0 ; --i)
            if(glmake_shaders_db.entries[i].path_hash == hash)
                return glmake_shaders_db.entries + i;
    return NULL;
}

GLuint glmake_compile_or_get_shader(const char *path){

    GLenum shtype;
    const char* ext = strrchr(path, '\0')-5;
    /**/ if(strcmp(ext, ".vert") == 0)
        shtype = GL_VERTEX_SHADER;
    else if(strcmp(ext, ".frag") == 0)
        shtype = GL_FRAGMENT_SHADER;
    else if(strcmp(ext, ".tesc") == 0)
        shtype = GL_TESS_CONTROL_SHADER;
    else if(strcmp(ext, ".tese") == 0)
        shtype = GL_TESS_EVALUATION_SHADER;
    else if(strcmp(ext, ".geom") == 0)
        shtype = GL_GEOMETRY_SHADER;
    else if(strcmp(ext, ".comp") == 0)
        shtype = GL_COMPUTE_SHADER;
    else {
        fprintf(stderr, "Unrecognised file extension from '%s'.\n", path);
        fprintf(stderr, "Recognised file extensions are ");
        fprintf(stderr, ".vert, .frag, .tesc, .tese, .geom and .comp.\n");
        return 0;
    }

    /* Check hashtable */
    uint64_t hash = glmake_hashfunc(path); /* reused later */
    struct glmake_shaders_db_entry *entry;
    entry = find_shaders_hashtable_entry(hash);
    if(entry != NULL)
        return entry->shader_id;

    FILE* file = fopen(path, "r");

    /* The weirdest thing in history. Without the following line, 
     * subsequent fread() calls return -1 with EINVAL if the file had been
     * opened and closed (!) before in the program's lifetime.
     * Very probably a MinGW issue. */
    strerror(errno);
    
    if(file == NULL) {
        fprintf(stderr, "Could not open '%s'\n", path);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);


    /* Not technically correct, but since file reading is expensive too,
     * the user should know what's going on. */
    printf("Compiling '%s'...\n", path); 

    long i;
    size_t bytes_read;
    GLchar **shsrc = malloc(((filesize/4096)+1)*sizeof(char*));
    for(i=0 ; i<filesize/4096 ; ++i) {
        shsrc[i] = malloc(4096+1);
        bytes_read = fread(shsrc[i], 1, 4096, file);
        if(bytes_read <= 0) {
            fclose(file);
            return 0;
        }
        shsrc[i][4096] = '\0';
    }
    shsrc[i] = malloc(filesize-(i*4096)+1);
    bytes_read = fread(shsrc[i], 1, 4096, file);
    if(bytes_read < 0)
        bytes_read = 0;
    shsrc[i][bytes_read] = '\0';
    fclose(file);

    GLuint shid = glCreateShader(shtype);
    glShaderSource(shid, i+1, (const GLchar* const*)shsrc, NULL);

    for(i=0 ; i<filesize/4096 ; ++i)
        free(shsrc[i]);
    free(shsrc[i]);
    free(shsrc);

    glCompileShader(shid);

    GLint status;
    glGetShaderiv(shid, GL_COMPILE_STATUS, &status);

    if(status == GL_TRUE) {
        glmake_shaders_db.entries = realloc(glmake_shaders_db.entries, 
                (glmake_shaders_db.top+1)*sizeof(struct glmake_shaders_db_entry));
        glmake_shaders_db.entries[glmake_shaders_db.top].path_hash = hash;
        glmake_shaders_db.entries[glmake_shaders_db.top].shader_id = shid;
        ++(glmake_shaders_db.top);
        return shid;
    }

    GLint errlen;
    glGetShaderiv(shid, GL_INFO_LOG_LENGTH, &errlen);
    char *err = malloc(errlen+1);
    glGetShaderInfoLog(shid, errlen, &errlen, err);
    err[errlen] = '\0';
    fprintf(stderr, "Failed to compile '%s' :\n%s\n ", path, err);
    free(err);
    glDeleteShader(shid);
    return 0;
}

void glmake_clean(void) 
{
    unsigned i;
    if(glmake_shaders_db.entries != NULL) {
        for(i=0 ; i<glmake_shaders_db.top ; ++i)
            glDeleteShader(glmake_shaders_db.entries[i].shader_id);
        free(glmake_shaders_db.entries);
        glmake_shaders_db.entries = NULL;
        glmake_shaders_db.top = 0;
    }
}

int glmake(GLuint program, const char *save_path, ...)
{
    const char *tmp_str;
    char *bin, *err;
    FILE *binfile;
    size_t bytes_read;
    GLsizei errlen;
    uint64_t binfile_mtime;
    GLenum binfmt;
    GLint status, binlen;
    GLuint shid;
    bool outdated;
    va_list ap;
    int retval;


    /* if opengl 4.1 */
    /* The appended 'b' to fopen()'s mode is actually critical for Windows. 
     * If omitted, the file's size is incorrectly interpreted. */
    binfile = fopen(save_path, "rb");
    if(binfile != NULL) 
    {
        /* Check if any shader file supplied has a more recent last write time 
         * than 'save_path'. In that case, 'save_path' is outdated and we should not 
         * read its binary. */
        binfile_mtime = glmake_last_write_time(save_path);
        va_start(ap, save_path);
        for(;;) 
        {
            tmp_str = va_arg(ap, const char*);
            if(tmp_str == NULL) 
                break;
            if(tmp_str[0] == '\0')
                continue;
            outdated = (binfile_mtime < glmake_last_write_time(tmp_str));
            if(outdated) {
                fclose(binfile);
                break;
            }
        }
        va_end(ap);

        if(!outdated)
        {
            fseek(binfile, 0, SEEK_END);
            binlen = ftell(binfile)-sizeof(GLenum);
            bin = malloc(binlen);
            rewind(binfile);
            bytes_read = fread(&binfmt, sizeof(GLenum), 1, binfile);
            bytes_read = fread(bin, 1, binlen, binfile);
            fclose(binfile);
            glProgramBinary(program, binfmt, bin, binlen);
            free(bin);

            if(glGetError() != GL_INVALID_ENUM) {
                glGetProgramiv(program, GL_LINK_STATUS, &status);
                if(status == GL_TRUE) {
                    printf("Successfully reused '%s'.\n", save_path);
                    return 1;
                }
            }
        
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errlen);
            err = malloc(errlen+1);
            glGetProgramInfoLog(program, errlen, &errlen, err);
            err[errlen] = '\0';
            fprintf(stderr, "Could not use '%s' :\n%s\n", save_path, err);
            free(err);
        }
    } /* binfile != NULL */
    /* endif opengl 4.1 */

    retval = 1;
    va_start(ap, save_path);
    for(;;)
    {
        tmp_str = va_arg(ap, const char *);
        if(tmp_str == NULL)
            break;
        if(tmp_str[0] == '\0')
            continue;
        shid = glmake_compile_or_get_shader(tmp_str);
        if(shid)
            glAttachShader(program, shid);
        else {
            fprintf(stderr, "Could not compile/reuse the shader.\n");
            retval = 0;
        }
    }
    va_end(ap);

    if(retval == 0)
        return retval;

    glLinkProgram(program);

    glGetProgramiv(program, GL_ATTACHED_SHADERS, &status);
    GLuint *shaders = malloc(status*sizeof(GLuint));
    glGetAttachedShaders(program, status, &status, shaders);
    for( ; status ; --status)
        glDetachShader(program, shaders[status-1]);
    free(shaders);

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_TRUE) {
        printf("Successfully linked '%s'.\n", save_path);
        glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binlen);
        bin = malloc(binlen);
        glGetProgramBinary(program, binlen, NULL, &binfmt, bin);
        binfile = fopen(save_path, "wb");
        if(!binfile) {
            fprintf(stderr, "Could not save binary to '%s'.\n", save_path);
        } else {
            fwrite(&binfmt, sizeof(GLenum), 1, binfile);
            fwrite(bin, 1, binlen, binfile);
            fclose(binfile);
        }
        free(bin);
    } else {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errlen);
        err = malloc(errlen+1);
        glGetProgramInfoLog(program, errlen, &errlen, err);
        err[errlen] = '\0';
        fprintf(stderr, "Failed to link '%s' :\n%s\n", save_path, err);
        free(err);
        retval = 0;
    }

    return retval;
}

