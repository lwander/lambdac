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

#include <stdbool.h>
#include <stddef.h>

MAKE_VOID_FREE(free_token)

/**
 * @brief Parse expression 
 *        <expression> ::= <var> | <lambda> | (<expression> <expression>)
 *
 * @param tokens Token buffer being parsed
 * @param cur Location of the start of the expression token
 * @param vars Variable context
 *
 * @return 0 on success, ERR_* otherwise
 */
int _parse_exp(dyn_buf_t *tokens, int cur, htable_t *vars) {
    return 0;
}

/**
 * @brief Attempt to parse a variable token
 *        <var> ::= [a-zA-Z0-9]*
 *
 * @param tokens The current token buffer being parsed
 * @param cur Location of the var token
 * @param vars Variable context (existing variables)
 * @param decl Is this a new variable being declared?
 * @param out Pointer to where the result should be stored (cannot be NULL)
 *
 * @return ERR_* on failure, 0 if no variable is being overwritten, otherwise
 *         a positive integer corresponding to the overrwritten variables ID
 */
int _parse_var(dyn_buf_t *tokens, int cur, htable_t *vars,
        int decl, var_t **out) {
    if (out == NULL)
        return ERR_INP;

    token_t read;
    int res;
    if ((res = dyn_buf_at(tokens, cur, (void *)&read)) < 0)
        return res;

    if (read.type != T_VAR)
        return ERR_BAD_PARSE;

    int id = 0;
    bool var_exists = (htable_lookup(vars, read.ident, &id) == 0);

    /* If a variable is not being bound anywhere, it is globally free and
     * we can't evaluate the program */
    if (!decl && !var_exists) {
        err_set_msg(read.ident);
        return ERR_UNBOUND_VAR;
    }

    /* Generate a fresh "id" for the variable if this is the variable's
     * binding site, otherwise it refers to its previous binding site */
    int new_id = id;
    if (decl)
        new_id = new_var_id();

    if ((*out = new_var(new_id, read.ident)) == NULL)
        return ERR_MEM_ALLOC;

    if ((res = htable_insert(vars, read.ident, new_id)) < 0)
        goto cleanup_var;

    /* This binding site shadows a previous one, so store the id of 
     * the previous binding site be restored when we leave this context */
    res = 0;
    if (decl && var_exists) {
        res = id;
        assert(id > 0);
    }

    return res;

cleanup_var:
    free_var(*out);

    return res;
}

/**
 * @brief Parse a lambda function construct
 *        <lambda> ::= (\<var>.<expression>)
 *
 * @param tokens Buffer of tokens being parsed
 * @param cur Index of token we expect lambda to start at
 * @param vars Variable context
 *
 * @return 0 on success, ERR_* otherwise
 */
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
    
    cur++;
    var_t *var;
    if ((res = _parse_var(tokens, cur, vars, 1, &var)) < 0) 
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
    dyn_buf_t *tokens = dyn_buf_new();

    int res;
    if ((res = lex(path, tokens)) < 0)
        goto cleanup_tokens;

    htable_t *vars = htable_new();

//cleanup_vars:
    htable_free(vars, NULL);

cleanup_tokens:
    dyn_buf_free(tokens, _free_token);

    return res;
}
