/**
 * @file lexer.c
 *
 * @brief lcc lexer implementation
 *
 * @author Lars Wander
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <err.h>
#include <util.h>

#include "lexer.h"

/**
 * @brief Pointer to new token struct
 *
 * @param sym Token type
 * @param ident NULL for non-identified tokens, otherwise should point to
 *        name. Memory will be copied for the token
 *
 * @return Pointer to token on success, NULL otherwise
 */
token_t *_new_token(token_e sym, char *ident) {
    token_t *res;
    if ((res = malloc(sizeof(token_t))) == NULL)
        goto cleanup_default;

    res->type = sym;

    if (ident != NULL) {
        size_t nlen;
        MIN(nlen, strlen(ident), MAX_VAR_LEN);
        if ((res->ident = malloc(nlen + 1)) == NULL)
            goto cleanup_ident;

        strncpy(res->ident, ident, nlen);
        res->ident[nlen] = '\0';
    } else {
        res->ident = NULL;
    }

    return res;

cleanup_ident:
    free(res);

cleanup_default:
    return NULL;
}

/**
 * @brief Free input token
 *
 * @param token Token to be freed
 */
void free_token(token_t *token) {
    if (token->ident != NULL)
        free(token->ident);

    free(token);
}

int _push_sym(dyn_buf_t *buf, token_e sym) {
    token_t *token;
    if ((token = _new_token(sym, NULL)) == NULL)
        return ERR_MEM_ALLOC;

    int res;
    if ((res = dyn_buf_push(buf, (void *)token)) < 0)
        goto cleanup_token;

    return 0;

cleanup_token:
    free_token(token);
    return res;
}


int _push_ident(dyn_buf_t *buf, token_e sym, char *ident) {
    token_t *token;
    if ((token = _new_token(sym, ident)) == NULL)
        return ERR_MEM_ALLOC;

    int res;
    if ((res = dyn_buf_push(buf, (void *)token)) < 0)
        goto cleanup_token;

    return 0;

cleanup_token:
    free_token(token);
    return res;
}

int lex(const char *path, dyn_buf_t *buf) {
    int res;
    if (path == NULL || buf == NULL) {
        res = ERR_INP;
        goto cleanup_default;
    }

    FILE *fp;
    if ((fp = fopen(path, "r")) == NULL) {
        res = ERR_FILE_ACTION;
        goto cleanup_default;
    }

    char ch;
    char *ident_buf;
    if ((ident_buf = calloc(1, MAX_VAR_LEN + 1)) == NULL) {
        res = ERR_MEM_ALLOC;
        goto cleanup_fp;
    }

    int ident_ind = 0;
    while ((ch = fgetc(fp)) != EOF) {
        if (isalnum(ch)) {
            ident_buf[ident_ind] = ch;
            ident_ind++;
            continue;
        }

        if (ident_ind > MAX_VAR_LEN) {
            ident_buf[MAX_VAR_LEN] = '\0';
            fprintf(stderr, "Identifier \"%s...\" too long\n", ident_buf);
            res = ERR_SEMANTICS;
            goto cleanup_buf;
        }

        if (ident_ind > 0) {
            ident_buf[ident_ind] = '\0';
            _push_ident(buf, T_VAR, ident_buf) ;
            ident_ind = 0;
        }

        switch (ch) {
            case ('('):
                _push_sym(buf, T_LPAREN);
                break;
            case (')'):
                _push_sym(buf, T_RPAREN);
                break;
            case ('.'):
                _push_sym(buf, T_DOT);
                break;
            case ('\\'):
                _push_sym(buf, T_BSLASH);
                break;
            case (' '):
            case ('\t'):
            case ('\r'):
            case ('\n'):
                break;
            default:
                fprintf(stderr, "Character %c not recognized\n", ch);
                res = ERR_SEMANTICS;
                goto cleanup_buf;
        }
    }

    res = 0;

cleanup_buf:
    free(ident_buf);

cleanup_fp:
    fclose(fp);

cleanup_default:
    return res;
}
