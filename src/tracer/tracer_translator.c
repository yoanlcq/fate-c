#define optional

struct op_node {
    const char *name;
};
typedef struct op_node op_node;

struct decl_node {
    const char *type;
    const char *name;
};
typedef struct decl_node decl_node;

struct sym_node {
    const char *name;
};
typedef struct sym_node sym_node;

struct val_node {
    const char *text;
};
typedef struct val_node val_node;

union node_union {
    op_node op;
    decl_node decl;
    sym_node sym;
    val_node val;
};

struct node;
typedef struct node node;

struct node {
    node_type type;
    union node_union data;
    optional node *left_child;
    optional node *rght_child;
};



/*
 * int cnt = 5 | printf("%s%d\n", foo(bar(21)), sin(42)+cos(12));
 * 
 * =
 * `- int cnt
 * `- |
 *    `- 5
 *    `- ()
 *       `- printf
 *       `- ,
 *          `- +
 *             `- ()
 *                `- sin
 *                `- 42
 *             `- ()
 *                `- cos
 *                `- 12
 *          `- ,
 *             `- "%s%d\n"
 *             `- ()
 *                `- foo
 *                `- ()
 *                   `- bar
 *                   `- 21
 * 
 * printf
 * `- "%s%d\n"
 * `- foo(bar(21))
 * `- sin(42)+cos(12)
 *
 * printf
 * `- "%s%d\n"
 * `- foo
 *    `- bar
 *       `- 21
 * `- sin + cos
 *    `- 42 `-21
 */
