/**
 * @file interpreter.c
 *
 * @brief Interpreter implementation. (Read Eval Print)
 *
 * @author Lars Wander (lwander)
 */

#include <stdio.h>

#include <err.h>

#include "lexer.h"
#include "parser.h"

const char *interp_prompt = "\\.> ";

int run_interp() {
    printf("%s", interp_prompt);

    dyn_buf_t *tokens = NULL;
    expr_t *ast = NULL;

    int res = 0;
    if ((res = lex(stdin, &tokens, '\n')) < 0) {
        goto cleanup;
    } else if ((res = parse(tokens, &ast)) < 0) {
        goto cleanup_tokens;
    } else {
#ifdef _DEBUG_
        if (dyn_buf_len(tokens) > 0) {
            printf("[intermediate state]\n");
            format_tokens(tokens);
            format_ast(ast);
        }
#endif /* _DEBUG_ */

    }

    free_expr(ast);

cleanup_tokens:
    dyn_buf_free(tokens, GET_VOID_FREE(free_token));

cleanup:
    return res;
}
