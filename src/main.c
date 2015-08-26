/**
 * @file main.c
 *
 * @brief entrypoint into lambda calculus compiler
 *
 * @author Lars Wander
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <lib/dyn_buf.h>
#include <err.h>
#include "parser.h"
#include "ast.h"
#include "interpreter.h"

const char *help = "Usage: lcc [options] [file]...\n"
"Options:\n"
"  -h         Display this message\n"
"  -i         Launch the interpreter\n";

int main(int argc, char **argv) {
    int interp = 0;
    char *fname = NULL;

    if (argc == 1) {
        printf("%s", help);
        exit(0);
    }

    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            interp = 1;
        } else {
            fname = argv[i];
        }
    }

    int res;
    if (fname != NULL) {
        expr_t *ast;
        if ((res = parse(fname, &ast)) < 0)
            printf("%s\n", err_to_string(res));
        else {
            format_ast(ast);
        }
    }

    if (interp) {
        while (run_interp() == 0) { }
    }

    return 0;
}
