/**
 * @file err.c
 *
 * @brief Error handling code
 *
 * @author Lars Wander
 */

#include <err.h>

#include <stdarg.h>
#include <stdio.h>

static const char *_msg;

void err_set_msg(const char *msg) {
    _msg = msg;
}

/**
 * @brief Print msg & error to STDERR
 *
 * @param msg Message with format specifiers for ... args
 * @param err Error code received
 * @param ... Variable arguments to be formatted inside msg
 *
 * @note You don't need to include a format specifier for the err in msg
 */
void err_report(const char *msg, int err, ...) {
    va_list ap;
    char err_wrap[128];
    char buf[256];

    va_start(ap, err);
    sprintf(err_wrap, "%s : %s\n", err_to_string(err), msg);
    vsnprintf(buf, sizeof(buf), err_wrap, ap);
    va_end(ap);

    fputs(buf, stderr);
}

/**
 * @brief Format an error code into a human readable string
 */
const char *err_to_string(int err) {
    switch (err) {
        case (ERR_OOB): 
            return "ERR_OOB";
        case (ERR_INP): 
            return "ERR_INP";
        case (ERR_CORRUPT): 
            return "ERR_CORRUPT";
        case (ERR_FILE_ACTION):
            return "ERR_FILE_ACTION";
        case (ERR_MEM_ALLOC):
            return "ERR_MEM_ALLOC";
        case (ERR_SEMANTICS):
            return "ERR_SEMANTICS";
        case (ERR_BAD_PARSE): 
            return "ERR_BAD_PARSE";
        case (ERR_UNBOUND_VAR):
            return "ERR_UNBOUND_VAR";
        case (0):
            return "NOT AN ERR";
        default:
            return "UNKNOWN ERR";
    }
}
