/**
 * @file parser.c
 *
 * @brief Parser implementation by recursive descent. For BNF see README.md
 *
 * The pointer to the current token being parsed follows the following
 * semantics: On input, it points to the index of the token starting the
 * construct being parsed. On an unsuccessful return, it is unchanged. On
 * a successful return, it points to the next token to be parsed.
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
#include <string.h>
#include <stdio.h>

MAKE_VOID_FREE(free_token)
int _parse_expr(dyn_buf_t *tokens, int *cur, htable_t *vars, expr_t **out);

/**
 * @brief shorthand for checking that the current token matches expectation
 *
 * @param tokens Token buffer being parsed
 * @param cur Pointer to token being verified
 * @param expected The token type we want to see
 * @param out If non-null and token type matches, a pointer to the token is
 *        placed here
 *
 * @return 0 on match, ERR_* in case of error, ERR_BAD_PARSE on non-match
 */
int _parse_verify_token(dyn_buf_t *tokens, int *cur, token_e expected,
        token_t *out) {
    int res;
    token_t *read;
    if ((res = dyn_buf_at(tokens, *cur, (void **)&read)) < 0)
        return res;

    if (read->type != expected)
        return ERR_BAD_PARSE;

    (*cur)++;
    if (out != NULL)
        memcpy(out, read, sizeof(token_t));

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
int _parse_var(dyn_buf_t *tokens, int *cur, htable_t *vars,
        int decl, var_t **out) {
    int _cur = *cur;
    if (out == NULL)
        return ERR_INP;

    token_t read;
    int res;
    if ((res = _parse_verify_token(tokens, &_cur, T_VAR, &read)) < 0)
        return res;

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

    /* Since we have successfully parsed the variable token, we update the
     * pointer to the token being parsed */
    *cur = _cur;

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
 * @param out Pointer to output lambda must point to valid memory
 *
 * @return 0 on success, ERR_* otherwise
 */
int _parse_lambda(dyn_buf_t *tokens, int *cur, htable_t *vars, lam_t **out) {
    int _cur = *cur;
    int res;
    /* ( */
    if ((res = _parse_verify_token(tokens, &_cur, T_LPAREN, NULL)) < 0)
        return res;

    /* \ */
    if ((res = _parse_verify_token(tokens, &_cur, T_BSLASH, NULL)) < 0)
        return res;

    /* <var> */
    var_t *var;
    if ((res = _parse_var(tokens, &_cur, vars, 1, &var)) < 0)
        return ERR_BAD_PARSE;
    int old_var_id = res;

    /* . */
    if ((res = _parse_verify_token(tokens, &_cur, T_DOT, NULL)) < 0)
        goto cleanup_var;

    /* <expr> */
    expr_t *expr;
    if ((res = _parse_expr(tokens, &_cur, vars, &expr)) < 0)
        goto cleanup_var;

    /* ) */
    if ((res = _parse_verify_token(tokens, &_cur, T_RPAREN, NULL)) < 0)
        goto cleanup_expr;

    if ((*out = new_lam(var, expr)) == NULL) {
        res = ERR_MEM_ALLOC;
        goto cleanup_expr;
    }

    *cur = _cur;

    /* Reset previous binding */
    if (old_var_id > 0) {
        htable_insert(vars, var->name, old_var_id);
    } else {
        htable_delete(vars, var->name, NULL);
    }

    return 0;

cleanup_expr:
    free_expr(expr);

cleanup_var:
    /* Reset previous binding */
    if (old_var_id > 0) {
        htable_insert(vars, var->name, old_var_id);
    } else {
        htable_delete(vars, var->name, NULL);
    }

    free_var(var);

    return res;
}

/**
 * @brief Parse function application
 *        (<expression> <expression>)
 *
 * @param tokens Token buffer being parsed
 * @param cur Location of the start of the application token
 * @param vars Variable context
 * @param out Pointer to output application, must be valid memory
 *
 * @return 0 on success, ERR_* otherwise
 */
int _parse_appl(dyn_buf_t *tokens, int *cur, htable_t *vars, appl_t **out) {
    int _cur = *cur;
    int res;

    /* ( */
    if ((res = _parse_verify_token(tokens, &_cur, T_LPAREN, NULL)) < 0)
        return res;

    /* <expression> */
    expr_t *lexpr;
    if ((res = _parse_expr(tokens, &_cur, vars, &lexpr)) < 0)
        return res;

    /* <expression> */
    expr_t *rexpr;
    if ((res = _parse_expr(tokens, &_cur, vars, &rexpr)) < 0)
        goto cleanup_lexpr;

    /* ) */
    if ((res = _parse_verify_token(tokens, &_cur, T_RPAREN, NULL)) < 0)
        goto cleanup_rexpr;

    if ((*out = new_appl(lexpr, rexpr)) == NULL) {
        res = ERR_MEM_ALLOC;
        goto cleanup_rexpr;
    }

    *cur = _cur;
    return 0;

cleanup_rexpr:
    free_expr(rexpr);

cleanup_lexpr:
    free_expr(lexpr);
    return res;
}

/**
 * @brief Parse expression
 *        <expression> ::= <var> | <lambda> | <application>
 *
 * @param tokens Token buffer being parsed
 * @param cur Location of the start of the expression token
 * @param vars Variable context
 * @param out Pointer to output expression, must be valid memory
 *
 * @return 0 on success, ERR_* otherwise
 */
int _parse_expr(dyn_buf_t *tokens, int *cur, htable_t *vars, expr_t **out) {
    int _cur = *cur;
    int res;

    /* Try to parse our 3 possibile expression types */
    var_t *var;
    if ((res = _parse_var(tokens, &_cur, vars, 0, &var)) == 0) {
        if ((*out = new_expr(VAR, (void *)var)) == NULL) {
            free_var(var);
            return ERR_MEM_ALLOC;
        } else {
            goto success;
        }
    } else if (res != ERR_BAD_PARSE) {
        return res;
    }

    lam_t *lam;
    if ((res = _parse_lambda(tokens, &_cur, vars, &lam)) == 0) {
        if ((*out = new_expr(LAMBDA, (void *)lam)) == NULL) {
            free_lam(lam);
            return ERR_MEM_ALLOC;
        } else {
            goto success;
        }
    } else if (res != ERR_BAD_PARSE) {
        return res;
    }

    appl_t *appl;
    if ((res = _parse_appl(tokens, &_cur, vars, &appl)) == 0) {
        if ((*out = new_expr(APPL, (void *)appl)) == NULL) {
            free_appl(appl);
            return ERR_MEM_ALLOC;
        } 
    } else {
        return res;
    }

success:
    *cur = _cur;
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

    format_tokens(tokens);

    htable_t *vars;
    if ((vars = htable_new()) == NULL)
        goto cleanup_tokens;

    int cur = 0;
    if ((res = _parse_expr(tokens, &cur, vars, ast)) < 0)
        goto cleanup_vars;

    if (cur != dyn_buf_len(tokens)) {
        res = ERR_BAD_PARSE;
        err_report("Trailing tokens from %d to %d", res, cur, 
                dyn_buf_len(tokens));
        goto cleanup_vars;
    }

    res = 0;;

cleanup_vars:
    htable_free(vars, NULL);

cleanup_tokens:
    dyn_buf_free(tokens, _free_token);

    return res;
}
