#define FE_SI_STRUCT(strct,content) \
    const fe_si_member fe_si_members_of_##strct[] = { content {0} }; \
    const fe_si_type fe_si_type_##strct = { \
        FE_SI_LQ_STRUCT, { \
            fe_si_members_of_##strct, \
            fe_countof(fe_si_members_of_##strct)-1 \
        }, #strct, sizeof(strct), sizeof(strct*), {0} \
    };
#define FE_SI_UNION(strct,content) \
    const fe_si_member fe_si_members_of_##strct[] = { content {0} }; \
    const fe_si_type fe_si_type_##strct = { \
        FE_SI_LQ_UNION, { \
            fe_si_members_of_##strct, \
            fe_countof(fe_si_members_of_##strct)-1 \
        }, #strct, sizeof(strct), sizeof(strct*), {0} \
    };
#define FE_SI_VAL(strct,type,name) \
    {&fe_si_type_##type, #name, offsetof(strct,name), {FE_SI_MB_VAL,{{0}}}, {0}},
#define FE_SI_PTR(strct,type,name) \
    {&fe_si_type_##type, #name, offsetof(strct,name), {FE_SI_MB_PTR,{{0}}}, {0}},
#define FE_SI_VEC(strct,type,name,cntloc,arg) \
    {&fe_si_type_##type, #name, offsetof(strct,name), {FE_SI_MB_VEC,.countgetter={cntloc,arg}}, {0}},
#define FE_SI_ARY(strct,type,name,cnt) \
    {&fe_si_type_##type, #name, offsetof(strct,name), {FE_SI_MB_ARY,.count=cnt}, {0}},
