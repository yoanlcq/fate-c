#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ast.h>

static void ast_node_destroy(ast_node *n) {
    if(n->left_child) ast_node_destroy(n->left_child);
    if(n->rght_child) ast_node_destroy(n->rght_child);
    switch(n->type) {
    case AST_OP_NODE:   if(n->data.op.name)   free(n->data.op.name);   break;
    case AST_DECL_NODE: if(n->data.decl.type) free(n->data.decl.type);
                        if(n->data.decl.name) free(n->data.decl.name); break;
    case AST_SYM_NODE:  if(n->data.sym.name)  free(n->data.sym.name);  break;
    }
    free(n);
}

static ast_node *ast_node_create(const char *in) {
    ast_node *n, *oldn;
    size_t skip;
    const char *left;

    n = calloc(1, sizeof(ast_node));

    while(isspace(*in)) ++in;

    if(is_unary_op(in, &skip)) {
        n->type = AST_OP_NODE;
        n->data.op.name = strndup(in, skip);

        in += skip;
        while(isspace(*in)) ++in;

        skip = 0;
        while(!is_op(in+skip, NULL)) ++skip;
        n->rght_child = calloc(1, sizeof(ast_node));
        n->rght_child.type = AST_SYM_NODE;
        n->rght_child.data.sym.name = skip ? strndup(in, skip) : NULL;
        oldn = n;
        n = ast_node_create(in+skip);
        n->left_child = oldn;
    }
    if(is_sym(in, &skip)||is_value(in, &skip));
        /* parse until op */

    return n;
}

void ast_init(ast *a, const char *instruction) {
    a->root = ast_node_create(instruction);
}
void ast_deinit(ast *a) {
    ast_node_destroy(a->root);
}
void ast_print_to(const ast *a, FILE *stream) {

}

