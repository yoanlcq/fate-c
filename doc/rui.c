struct rui_color {
    uint8_t r,g,b,a;
};
typedef struct rui_color rui_color;

struct rui_2d {
    uint16_t x, y;
};
typedef struct rui_2d rui_2d;

enum rui_icon {
    RUI_ICON_NONE = 0,
    RUI_ICON_CROSS,
    RUI_ICON_VALIDATE,
    RUI_ICON_CLOCK
};
typedef enum rui_icon rui_icon;

enum rui_role {
    RUI_LABEL = 0,
    RUI_BUTTON,
    RUI_FIELD,
    RUI_SLIDER,
};
typedef enum rui_role rui_role;

enum rui_part {
    RUI_CENTER = 0;
    RUI_EAST,
    RUI_NORTHEAST,
    RUI_NORTH,
    RUI_NORTHWEST,
    RUI_WEST,
    RUI_SOUTHWEST,
    RUI_SOUTH,
    RUI_SOUTHEAST
};
typedef enum rui_part rui_part;


struct rui_dropdown_item {
    rui_icon icon;
    uint8_t *text;
    uint8_t *help;
};
typedef struct rui_dropdown_item rui_dropdown_item;

/* 
 * 'draw' IS the theme. It'd be a global func.
 * To change theme, change the function pointer.
 */
struct rui_widget {
    void    (*draw)(const rui_widget *widget, rui_color **rect_fillme, const rui_2d *size);
    rui_2d    pos_chars;
    rui_2d    siz_chars;
    rui_icon  icon;
    uint8_t  *text;
    uint8_t  *description;
    rui_role  role;
    float     progress;
    void (*draw_slider)(rui_color **fillme, rui_2d size, float progress);
    rui_dropdown_item *dropdown_items;
};
typedef struct rui_widget rui_widget;

