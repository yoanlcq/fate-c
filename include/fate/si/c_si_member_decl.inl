#define FE_SI_STRUCT(strct,content) \
    extern const fe_si_member fe_si_members_of_##strct[]; \
    extern const fe_si_type fe_si_type_##strct;
#define FE_SI_UNION(strct,content) FE_SI_STRUCT(strct,content)
#define FE_SI_VAL(strct,type,name)           
#define FE_SI_PTR(strct,type,name)           
#define FE_SI_VEC(strct,type,name,cntloc,arg)
#define FE_SI_ARY(strct,type,name,cnt)       
