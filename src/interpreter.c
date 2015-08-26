/**
 * @file interpreter.c
 *
 * @brief Interpreter implementation. (Read Eval Print)
 *
 * @author Lars Wander (lwander)
 */

#include <stdio.h>

const char *interp_prompt = "\\.> ";
const char *interp_start_msg = "Type exit to quit\n";

int run_interp() {
    printf("%s", interp_start_msg);
    printf("%s", interp_prompt);
    return 0;
}
