/**
 * @file ast.h 
 *
 * @brief Lambda calculus AST definition 
 *
 * @author Lars Wander
 */

#ifndef _AST_H_
#define _AST_H_

/**
 * @brief Var data format - comparison is done by `id` field rather than
 *        `name` for variable name shadowing
 */
typedef struct _var {
    /* Identifier for variable with this `name` & context */
    unsigned int id; 

    /* For printing purposes */
    char *name;
} var_t;

typedef enum _expr_e {
    VAR,
    LAMBDA,
    APPL
} expr_e;

/**
 * @brief Expression AST node, can be VAR, LAMBDA, or APPL - designated by
 *        `type` field.
 */
typedef struct _expr {
    /* See `_expr_e` enum */
    expr_e type; 
    
    /* To be cast based on `type` field */
    void *data; 
} expr_t;

/**
 * @brief Data representation of a lambda AST node 
 */
typedef struct _lam {
    /* Free variable bound by this lambda */
    var_t *var;

    /* Function body */
    expr_t *body;      
} lam_t;

/**
 * @brief Data represntation for function application, or f x
 */
typedef struct _appl {
    expr_t *f;
    expr_t *x;
} appl_t;

unsigned int new_var_id();

var_t *new_var(unsigned int id, const char *name);
expr_t *new_expr(expr_e type, void *data);
lam_t *new_lam(var_t *var, expr_t *body);
appl_t *new_appl(expr_t *f, expr_t *x);
expr_t *deep_copy_expr(expr_t *e);
void free_var(var_t *var);
void free_expr(expr_t *expr);
void free_lam(lam_t *lam);
void free_appl(appl_t *appl);
void format_ast(expr_t *expr);

#endif /* _AST_H_ */
