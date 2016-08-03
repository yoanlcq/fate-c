#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    if(argc <= 1) {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *inf = fopen(argv[1], "rb");
    if(!inf) {
        fprintf(stderr, "Could not open %s !\n", argv[1]);
        return EXIT_FAILURE;
    }

    char *slash = strrchr(argv[1], '/');
    if(!slash)
        slash = strrchr(argv[1], '\\');
    if(!slash)
        slash = argv[1]-1;
    char *shid = strdup(slash+1);
    for(;;) {
        char *dot = strchr(shid, '.');
        if(!dot)
            break;
        *dot = '_';
    }
    char *shid_up = strdup(shid);
    size_t i, len;
    len = strlen(shid_up);
    for(i=0 ; i<len ; ++i)
        shid_up[i] = toupper(shid_up[i]);
    FILE *outf = stdout;
    const char *prefix = "fe_gl_shadersrc_";
#define INDENT "    "
    fprintf(outf,
            "#ifndef %s_H\n"
            "#define %s_H\n"
            "static const char *const %s%s = {\n" INDENT,
            shid_up, shid_up, prefix, shid);
    int c, mod = 1;
    unsigned long count = 0;
    while((c = fgetc(inf)) != EOF) {
        printf("0x%02hhx,%s", c, mod ? " " : "\n" INDENT);
        ++count;
        ++mod;
        mod %= 8;
    }
    fclose(inf);
    fprintf(outf, 
            "0x00\n"
            "};\n"
            "static const unsigned long %s%s_strlen = %lu\n"
            "#endif /* %s_H */", 
            prefix, shid, shid_up, count);
    fclose(outf);
    free(shid);
    free(shid_up);
    return EXIT_SUCCESS;
}
