/**
 * @file main.c
 *
 * @brief entrypoint into lambda calculus compiler
 *
 * @author Lars Wander
 */

#include <stdio.h>

#include <lib/dyn_buf.h>
#include "lexer.h"

int main(int argc, char **argv) {
    dyn_buf_t *buf = dyn_buf_new();
    int res;
    if ((res = lex(argv[1], buf)) < 0)
        printf("%d\n", res);

    format_tokens(buf);

    return 0;
}
