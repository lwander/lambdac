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
#include "lexer.h"
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

    int res = 0;
    if (fname != NULL) {
        FILE *fp = NULL;
        if ((fp = fopen(fname, "r")) == NULL) {
            err_report("Failed to open %s", ERR_FILE_ACTION, fname);
            return -1;
        }

        dyn_buf_t *tokens;
        expr_t *ast;
        if ((res = lex(fp, &tokens, EOF)) < 0) {
            goto cleanup_fp;
        } else if ((res = parse(tokens, &ast)) < 0) {
            goto cleanup_tokens;
        } else {
#ifdef _DEBUG_
            /*
            format_tokens(tokens);
            format_ast(ast); */
#endif /* _DEBUG_ */
        }

        do {
            printf("\x1B[34m-\033[0m ");
            format_ast(ast);
        } while (step_expr(ast) == 0);

        free_expr(ast);

cleanup_tokens:
        dyn_buf_free(tokens, GET_VOID_FREE(free_token));

cleanup_fp:
        fclose(fp);
    }

    if (interp) {
        while ((res = run_interp()) != 1) { }
    }


    return res;
}
