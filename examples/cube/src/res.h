#ifndef RES_LIST_H
#define RES_LIST_H

typedef struct {
    char *bk;
    char *dn;
    char *ft;
    char *lf;
    char *rt;
    char *up;
} res_cubemap;

typedef struct {
    struct {
        res_cubemap flame;
    } cubemaps;
    struct {
        const char *deeper;
    } musics;
} res_list;

extern const res_list res;

#endif /* RES_LIST_H */
