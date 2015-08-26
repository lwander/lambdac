/**
 * @file lexer.h
 *
 * @brief Token & lexer definitions go here
 *
 * @author Lars Wander
 */

#ifndef _LEXER_H_
#define _LEXER_H_

#include <stdio.h>

#include <lib/dyn_buf.h>

#define MAX_VAR_LEN 64

/**
 * @brief All types of tokens enumerated
 */
typedef enum _token_e {
    T_LPAREN, /* ( */
    T_RPAREN, /* ) */
    T_BSLASH, /* \ */
    T_DOT, /* . */
    T_VAR  /* variable name */
} token_e;

/**
 * @brief Token struct for representing every type of token
 */
typedef struct _token {
    /* Represents type of the token */
    token_e type;

    /* NULL whenever non-VAR type token */
    char *ident;
} token_t;

MAKE_VOID_FREE_DEF(free_token)

int lex(FILE *fp, dyn_buf_t **buf, char eof);
void format_tokens(dyn_buf_t *buf);
void free_token(token_t *token);


#endif /* _LEXER_H_ */
