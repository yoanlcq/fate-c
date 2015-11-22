#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

//"(\"(foobar)";
//
void remove_C_comments_and_preprocessor_directives(char *src) {

    for( ; *src ; ++src) {
        switch(*src) {
        case '\'': 
            if(src[1]=='\\' && src[2]=='\'')
                src += 3;
            else do ++src; while(*src!='\'');
            break;
        case '"':
            do ++src; while(!(*src=='"' && src[-1]!='\\'));
            break;
        case '/':
            if(src[1]=='/') {
                src[0] = src[1] = ' ';
                ++src;
                for(++src ; *src!='\n' ; ++src)
                    *src = ' ';
            } else if(src[1]=='*') {
                src[0] = src[1] = ' ';
                ++src;
                for(++src ; !(*src=='*' && src[1]=='/') ; ++src)
                    *src = ' ';
                src[0] = src[1] = ' ';
                ++src;
            }
            break;
        case '#': 
            *src = ' ';
            for(++src ; *src!='\n' ; ++src) {
                if(*src=='\\' && src[1]=='\n') {
                    src[0] = src[1] = ' ';
                    ++src;
                } else *src = ' ';
            }
            break;
        }
    }
}

size_t readfile(const char *filename, char **buf) {
    char tmp_buf[BUFSIZ];
    size_t bytes_read;
    size_t buf_len;
    FILE *in = fopen(filename, "r");

    if(!in) {
        fprintf(stderr, "Could not open '%s' for reading.\n", filename);
        return 0;
    }

    for(*buf=NULL, buf_len=0 ; ; ) {
        bytes_read = fread(tmp_buf, 1, BUFSIZ, in);
        if(bytes_read <= 0)
            break;
        *buf = realloc(*buf, buf_len+bytes_read);
        memcpy((*buf)+buf_len, tmp_buf, bytes_read);
        buf_len += bytes_read;
    }
    fclose(in);
    *buf = realloc(*buf, buf_len+1);
    (*buf)[buf_len] = '\0';

    return buf_len;
}

int is_C_qualifier(const char *str, size_t n) {
#define HELPER(KW) \
    if(!strncmp(str, #KW, n)) return 1;
    HELPER(auto);
    HELPER(char);
    HELPER(const);
    HELPER(double);
    HELPER(enum);
    HELPER(extern);
    HELPER(float);
    HELPER(int);
    HELPER(long);
    HELPER(register);
    HELPER(short);
    HELPER(signed);
    HELPER(static);
    HELPER(struct);
    HELPER(typedef);
    HELPER(union);
    HELPER(unsigned);
    HELPER(void);
    HELPER(volatile);
    HELPER(__attribute__);
#undef HELPER
    return 0;
}

int is_C_operator(const char *str, size_t n) {
#define HELPER(KW) if(!strncmp(str, #KW, n)) return 1;
    HELPER(sizeof);
#undef HELPER
    return 0;
}

int is_C_ctrl(const char *str, size_t n) {
#define HELPER(KW) if(!strncmp(str, #KW, n)) return 1;
    HELPER(break);
    HELPER(case);
    HELPER(continue);
    HELPER(default);
    HELPER(do);
    HELPER(else);
    HELPER(for);
    HELPER(goto);
    HELPER(if);
    HELPER(return);
    HELPER(switch);
    HELPER(while);
#undef HELPER
    return 0;
}

int is_C_keyword(const char *str, size_t n) {
    return is_C_qualifier(str, n) 
        || is_C_operator(str, n)
        || is_C_ctrl(str, n);
}

void print_type(const char *left, size_t n) {
    int skipping;
    const char *beg = left;
    for(; left<beg+n ; ++left) {
        if(isalnum(*left) || *left=='*' || *left=='_') {
            skipping = 0;
        } else {
            if(!skipping) {
                putchar(' ');
                skipping = 1;
            }
        }
        if(!skipping)
            putchar(*left);
    }
}

const char *memmem(const char *haystack, size_t hlen, 
                   const char *needle, size_t nlen) {
    if (nlen == 0) return haystack; /* degenerate edge case */
    if (hlen < nlen) return 0; /* another degenerate edge case */
    const char *hlimit = haystack + hlen - nlen + 1;
    while (haystack = memchr(haystack, needle[0], hlimit-haystack)) {
        if (!memcmp(haystack, needle, nlen)) return haystack;
        ++haystack;
    }
    return 0;
}

int print_decl(const char *buf, const char *right) {
    for( ; right >= buf && isspace(*right) ; --right);

    if(!(isspace(*right) || isalnum(*right) || *right=='*' || *right=='_'))
        return 0;
    
    const char *left = right;

    for( ; left >= buf ; --left)
        if(!(isspace(*left) || isalnum(*left) || *left=='*' || *left=='_'))
            break;

    if(*left=='#')
        return 0;

    do ++left; while(isspace(*left));
    if(!(right-left+1))
        return 0;
    if(is_C_ctrl(left, right-left+1) || is_C_operator(left, right-left+1))
        return 0;
    if(memmem(left, right-left+1, "typedef", strlen("typedef")))
        return 0;
    printf("DECL: ");
    print_type(left, right-left+1);
    printf(" : ");
    return 1;
}

int first_word_is_C_keyword(const char *left, size_t n) {
    for( ; n && isspace(*left) ; ++left, --n);
    const char *right = left;
    for( ; n && !isspace(*right) ; ++right, --n);
    return is_C_keyword(left, right-left);
}
int print_prefix(const char *buf, const char *right) {
    const char *left = right;
    
    for( ; left >= buf && isspace(*left) ; --left);
    for( ; left >= buf && (isalnum(*left) || *left=='_') ; --left);
    ++left;

    if(is_C_keyword(left, right-left+1))
        return 0;
    if(print_decl(buf, left-1)) {
        fwrite(left, 1, right-left+1, stdout);
        return 1;
    }
    /* TODO : register user types in the C keywords list. */
    if(first_word_is_C_keyword(left, right-left+1))
        return 0;
    printf("CALL: ");
    fwrite(left, 1, right-left+1, stdout);
    return 1;
}

int is_chared(const char *p) {
    return (p[-1]=='\'' || (p[-2]=='\'' && p[-1]=='\\')) && p[1]=='\'';
}

void foobar(void) {}

int str_is_white(const char *s, size_t n) {
    const char *beg = s;
    for( ; s<beg+n ; ++s)
        if(!isspace(*s))
            return 0;
    return 1;
foobar(  );
}

/* Must be ignored for function declarations, since it would treat 
 * 'void' as a parameter. */
unsigned get_numargs(const char *left, size_t n) {
    if(!n || str_is_white(left, n))
        return 0;

    const char *beg = left;
    unsigned numargs = 1;
    unsigned stack = 0;

    for( ; left<beg+n ; ++left) {
        if(*left=='(')
            ++stack;
        else if(stack && *left==')')
            --stack;
        else if(!stack && *left==',')
            ++numargs;
    }

    return numargs;
}

void print_function_calls(const char *buf, size_t buf_len) {
    unsigned stack, numargs;
    const char *left, *right;
    for(stack=0, left=buf ; left < buf+buf_len ; ++left) {
        if(*left == '"' && !is_chared(left))
            do ++left; while(!(*left == '"' && left[-1] != '\\'));
        else if(*left == '(' && !is_chared(left)) {
            ++stack;
            for(right = left+1 ; right < buf+buf_len ; ++right) {
                if(*right == '"' && !is_chared(right))
                    do ++right; while(!(*right == '"' && right[-1] != '\\'));
                else if(*right == '(' && !is_chared(right))
                    ++stack;
                else if(*right == ')' && !is_chared(right)) {
                    --stack;
                    if(!stack) {
                        if(print_prefix(buf, left-1)) {
                            printf(" : ");
                            fwrite(left, 1, right-left+1, stdout);
                            numargs = get_numargs(left+1, right-left-1);
                            printf(" : %u arg%s", numargs, 
                                    numargs!=1 ? "s" : "");
                            fputc('\n', stdout);
                        }
                        left = right;
                        break;
                    }
                }
            }
        }
    }
}

#include <ast.h>

int main(int argc, char *argv[]) {

    if(argc <= 1) {
        fputs("Usage: %s <filename>\n", stderr);
        exit(EXIT_FAILURE);
    }

    char *buf;
    size_t buf_len = readfile(argv[1], &buf);

    if(!buf_len) {
        fputs("A problem has been encountered. Exiting.\n", stderr);
        exit(EXIT_FAILURE);
    }

    remove_C_comments_and_preprocessor_directives(buf);
    //puts(buf);

    ast ast;
    ast_init(&ast, 
        "int cnt = 5 | "
        "printf(\"%s%d\\n\", foo(bar(21)), sin(42)+cos(12))");
    ast_print_to(&ast, stdout);
    ast_deinit(&ast);

    //print_function_calls(buf, buf_len);

    free(buf);

    return EXIT_SUCCESS;
}
