/**
 * @file parser.c
 *
 * @brief Parser implementation by recursive descent. For BNF see README.md
 *
 * @author Lars Wander
 */

#include "parser.h"
#include "lexer.h"

#include <err.h>
#include <lib/dyn_buf.h>
#include <lib/hashtable.h>

MAKE_VOID_FREE(free_token)

int _parse_exp(dyn_buf_t *tokens, int cur, htable_t *vars) {
    return 0;    
}

int _parse_var(dyn_buf_t *tokens, int cur, htable_t *vars) {
    return 0;
}

int _parse_lambda(dyn_buf_t *tokens, int cur, htable_t *vars) {
    token_t read;
    int res;
    if ((res = dyn_buf_at(tokens, cur, (void *)&read)) < 0)
        return res;

    if (read.type != T_LPAREN)
        return ERR_BAD_PARSE;

    cur++;
    if ((res = dyn_buf_at(tokens, cur, (void *)&read)) < 0)
        return res;

    if (read.type != T_BSLASH)
        return ERR_BAD_PARSE;
     
    return 0;
}

/**
 * @brief Parse LC file at path
 *
 * @param path LC file
 * @param pointer to where AST will be stored, cannot be NULL
 *
 * @return 0 on success, ERR_* otherwise
 */
int parse(const char *path, expr_t **ast) {
    dyn_buf_t *tokens = new_dyn_buf();

    int res;
    if ((res = lex(path, tokens)) < 0)
        goto cleanup_tokens;

cleanup_tokens:
    free_dyn_buf(tokens, _free_token);   

    return res;
}
