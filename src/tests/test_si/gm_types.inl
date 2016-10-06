FE_SI_STRUCT(color_rgb,
    FE_SI_VAL(color_rgb,uint8_t,r)
    FE_SI_VAL(color_rgb,uint16_t,g)
    FE_SI_VAL(color_rgb,uint8_t,b)
)

FE_SI_STRUCT(gr_grisui,
    FE_SI_PTR(gr_grisui,gr_grisui,parent)
    FE_SI_ARY(gr_grisui,uint16_t,spells,4)
    FE_SI_VEC(gr_grisui,uint16_t,items,static_get_item_count,NULL)
    FE_SI_VAL(gr_grisui,color_rgb,color)
)
