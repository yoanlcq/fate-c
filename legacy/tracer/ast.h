#ifndef AST_H
#define AST_H

struct ast_op_node {
    char *name;
};
typedef struct ast_op_node ast_op_node;

struct ast_decl_node {
    char *type;
    char *name;
};
typedef struct ast_decl_node ast_decl_node;

struct ast_sym_node {
    char *name;
};
typedef struct ast_sym_node ast_sym_node;

enum ast_node_type {
    AST_EMPTY_NODE = 0,
    AST_OP_NODE    = 1,
    AST_DECL_NODE  = 2,
    AST_SYM_NODE   = 3,
};
typedef enum ast_node_type ast_node_type;

union ast_node_union {
    ast_op_node op;
    ast_decl_node decl;
    ast_sym_node sym;
};

struct ast_node;
typedef struct ast_node ast_node;

struct ast_node {
    ast_node_type type;
    union ast_node_union data;
    ast_node *left_child;
    ast_node *rght_child;
};

struct ast {
    ast_node *root;
};
typedef struct ast ast;

void ast_init(ast *a, const char *instruction);
void ast_deinit(ast *a);
void ast_print_to(const ast *a, FILE *stream);

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

#endif /* AST_H */
