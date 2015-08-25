/**
 * @file ast.c
 *
 * @brief AST manipulation functions go here
 *
 * @author Lars Wander
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <util.h>

#include "ast.h"
#include "lexer.h"

static unsigned int _var_id = 1;

/**
 * @brief Get a fresh var id for alpha equivalence
 *
 * @return a new variable id
 */
unsigned int new_var_id() {
    return _var_id++;
}

/**
 * @brief Return a new variable struct
 *
 * @param id ID of the incoming variable
 * @param name Name of the incoming variable (will be copied)
 *
 * @return New variable struct
 */
var_t *new_var(unsigned int id, const char *name) {
    var_t *res;
    if ((res = malloc(sizeof(var_t))) == NULL)
        goto cleanup_default;

    res->id = id;

    size_t nlen;
    MIN(nlen, strlen(name), MAX_VAR_LEN);
    if ((res->name = malloc(nlen + 1)) == NULL)
        goto cleanup_var;

    strncpy(res->name, name, nlen);
    res->name[nlen] = '\0';

    return res;

cleanup_var:
    free(res);

cleanup_default:
    return NULL;
}

/**
 * @brief Delete a var struct
 */
void free_var(var_t *var) {
    if (var == NULL)
        return;

    if (var->name != NULL)
        free(var->name);

    free(var);
}

/**
 * @brief Return a new lamdba struct
 *
 * @param var Variable being bound inside this lambda
 * @param body Expression inside lambda body
 *
 * @return New lambda struct
 */
lam_t *new_lam(var_t *var, expr_t *body) {
    lam_t *res;
    if ((res = malloc(sizeof(lam_t))) == NULL)
        return NULL;

    res->var = var;
    res->body = body;
    return res;
}

/**
 * @brief Delete lam struct
 */
void free_lam(lam_t *lam) {
    if (lam == NULL)
        return;

    if (lam->body != NULL)
        free_expr(lam->body);

    if (lam->var != NULL)
        free_var(lam->var);

    free(lam);
}

/**
 * @brief Return a new function application struc
 *
 * @param f Function being acted on
 * @param x Function being applied
 *
 * @return New appl struc
 */
appl_t *new_appl(expr_t *f, expr_t *x) {
    appl_t *res;
    if ((res = malloc(sizeof(appl_t))) == NULL)
        return NULL;

    res->f = f;
    res->x = x;
    return res;
}

/**
 * @brief Delete appl struc
 */
void free_appl(appl_t *appl) {
    if (appl == NULL)
        return;

    if (appl->f != NULL)
        free_expr(appl->f);

    if (appl->x != NULL)
        free_expr(appl->x);

    free(appl);
}

/**
 * @brief Return a new expression struct
 *
 * @param type type of expression
 * @param data pointer to expression of type `type`
 *
 * @return New expr struc
 */
expr_t *new_expr(expr_e type, void *data) {
    expr_t *res;
    if ((res = malloc(sizeof(expr_t))) == NULL)
        return NULL;

    res->data = data;
    res->type = type;
    return res;
}

/**
 * @brief Delete expr struct
 */
void free_expr(expr_t *expr) {
    if (expr == NULL)
        return;

    switch (expr->type) {
        case (VAR):
            free_var((var_t *)expr->data);
            break;
        case (LAMBDA):
            free_lam((lam_t *)expr->data);
            break;
        case (APPL):
            free_appl((appl_t *)expr->data);
            break;
        default:
            fprintf(stderr, "Corrupted expression node");
            exit(1);
    }

    free(expr);
}

void _format_expr(expr_t *expr);
    
/**
 * @brief Format input variable 
 */
void _format_var(var_t *var) {
    printf("( [var] \"%s\".%d )", var->name, var->id);
}

/**
 * @brief Format input lambda 
 */
void _format_lam(lam_t *lam) {
    printf("( [lam] \\");
    _format_var(lam->var);
    printf(".");
    _format_expr(lam->body);
    printf(" )");
}

/**
 * @brief Format input application
 */
void _format_appl(appl_t *appl) {
    printf("( [appl] ");
    _format_expr(appl->f);
    printf(" ");
    _format_expr(appl->x);
    printf(" )");
}


/**
 * @brief Format input expression 
 */
void _format_expr(expr_t *expr) {
    printf("( [expr] ");
    switch (expr->type) {
        case (VAR):
            _format_var((var_t *)expr->data);
            break;
        case (LAMBDA):
            _format_lam((lam_t *)expr->data);
            break;
        case (APPL):
            _format_appl((appl_t *)expr->data);
            break;
        default:
            printf("??? %d", expr->type);
    }
    printf(" )");
}

/**
 * @brief Format input AST
 */
void format_ast(expr_t *expr) {
    _format_expr(expr);
    printf("\n");
}

