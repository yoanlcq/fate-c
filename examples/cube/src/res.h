#ifndef RES_LIST_H
#define RES_LIST_H

typedef struct { char *jpg; } res_jpg;
typedef struct { char *ogg; } res_ogg;

typedef struct {
    res_jpg bk;
    res_jpg dn;
    res_jpg ft;
    res_jpg lf;
    res_jpg rt;
    res_jpg up;
} res_cubemap_jpg;

typedef struct {
    struct {
        res_cubemap_jpg flame;
    } cubemaps;
    struct {
        res_ogg deeper;
        res_ogg wind;
    } musics;
} res_list;

extern const res_list res;
fe_fpath res_path(const char *filepath);
bool res_load(fe_iov *iov, const char *filepath);


#endif /* RES_LIST_H */
