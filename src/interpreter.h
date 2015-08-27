/**
 * @file interpreter.h
 *
 * @brief Interpreter top level declarations go here
 *
 * @author Lars Wander
 */

#ifndef _INTERPERTER_H_
#define _INTERPERTER_H_

#include "ast.h"

int step_expr(expr_t *expr);
int run_interp();

#endif /* _INTERPERTER_H_ */
