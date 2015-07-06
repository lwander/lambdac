/**
 * @file parser.h
 *
 * @brief Parser declaration 
 *
 * @author Lars Wander
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "ast.h"

int parse(const char *path, expr_t **ast);

#endif /* _PARSER_H_ */
