#define FE_SI_STRUCT(strct,content) typedef struct strct strct; struct strct { content };
#define FE_SI_UNION(strct,content)  typedef union strct strct;  union strct { content };
#define FE_SI_VAL(strct,type,name)            type name;
#define FE_SI_PTR(strct,type,name)            type *name;
#define FE_SI_VEC(strct,type,name,cntloc,arg) type *name;
#define FE_SI_ARY(strct,type,name,cnt)        type name[cnt];
