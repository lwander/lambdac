/**
 * @file parser.h
 *
 * @brief Parser declaration 
 *
 * @author Lars Wander
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include <lib/dyn_buf.h>

#include "ast.h"

int parse(dyn_buf_t *buf, expr_t **ast);

#endif /* _PARSER_H_ */
