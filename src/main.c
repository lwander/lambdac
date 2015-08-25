/**
 * @file main.c
 *
 * @brief entrypoint into lambda calculus compiler
 *
 * @author Lars Wander
 */

#include <stdio.h>

#include <lib/dyn_buf.h>
#include <err.h>
#include "parser.h"
#include "ast.h"

int main(int argc, char **argv) {
    int res;
    expr_t *ast;
    if ((res = parse(argv[1], &ast)) < 0)
        printf("%s\n", err_to_string(res));
    else {
        format_ast(ast);
    }

    return 0;
}
