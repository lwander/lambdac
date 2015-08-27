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
#include "ast.h"

const char *interp_prompt = "\x1B[34m\xCE\xBB.>\033[0m ";
int step_expr(expr_t *expr);

/**
 * @brief Propagate substitution through expression. We provide a double 
 *        pointer because we need to replace var expression structs once 
 *        we encounter them.
 *
 * @param expr The expression where substitution is happening
 * @param id The id of the variable we are substituting into 
 * @param x the new data being substituted in
 *
 * @return 0 on success, ERR_* othewise
 */
int subst_var(expr_t **expr, unsigned int id, expr_t *x) {
    int res;
    switch ((*expr)->type) {
        case (VAR):
            if (((var_t *)((*expr)->data))->id == id) {
                free_expr(*expr);
                *expr = deep_copy_expr(x);
            } 
            return 0;
        case (LAMBDA):
            return subst_var(((expr_t **)&((lam_t *)(*expr)->data)->body), 
                    id, x);
        case (APPL):
            if ((res = subst_var(((expr_t **)&((appl_t *)(*expr)->data)->f), 
                    id, x)) < 0)
                return res;
            return subst_var(((expr_t **)&((appl_t *)(*expr)->data)->x), 
                    id, x);
        default:
            return ERR_BAD_PARSE;
    }
}

/**
 * @brief Apply an expression to another. The reason we don't pass the appl_t
 *        struct inside the expr_t struct is that it's contents will change,
 *        and we want the reference to reflect that change
 *
 * @param expr The application expressoin
 *
 * @return 0 on success, ERR_* otherwise
 */
int appl_expr(expr_t *expr) {
    if (expr->type != APPL)
        return ERR_INP;

    appl_t *appl = (appl_t *)expr->data;
    if (appl->f->type != LAMBDA)
        if (step_expr(appl->f) == 1)
            return step_expr(appl->x);

    lam_t *lam = (lam_t *)appl->f->data;

    unsigned int var_id = lam->var->id;

    int res;
    if ((res = subst_var(&lam->body, var_id, appl->x)) < 0) 
        return res;
    
    /* Replace expression data with new program data */
    expr->type = lam->body->type; 
    expr->data = lam->body->data; 
    /* After this, our appl struct is appl(lam([unused var], NULL), [copied
     * expr]), which we can safely free */
    lam->body = NULL;
    free_appl(appl);

    return 0;
}

/**
 * @brief Single step input expression
 *
 * @param Expression to be stepped (will be modified)
 *
 * @return ERR_* on error, 0 on success, 1 if no step can be taken
 */
int step_expr(expr_t *expr) {
    switch (expr->type) {
        case (VAR):
            return 1;
        case (LAMBDA):
            return step_expr((expr_t *)((lam_t *)expr->data)->body);
        case (APPL):
            return appl_expr(expr);
        default:
            return ERR_BAD_PARSE;
    }
}

/**
 * @brief Run one read eval print step
 */
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
        /*
        if (dyn_buf_len(tokens) > 0) {
            printf("[intermediate state]\n");
            format_tokens(tokens);
            format_ast(ast);
        } */
#endif /* _DEBUG_ */

    }

    do { 
        printf("\x1B[34m-\033[0m ");
        format_ast(ast);
    } while (step_expr(ast) == 0);

    free_expr(ast);

cleanup_tokens:
    dyn_buf_free(tokens, GET_VOID_FREE(free_token));

cleanup:
    return res;
}
