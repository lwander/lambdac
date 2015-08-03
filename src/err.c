/**
 * @file err.c
 *
 * @brief Error handling code
 *
 * @author Lars Wander
 */

static const char *_msg;

void err_set_msg(const char *msg) {
    _msg = msg;
}
