/**
 * @file dyn_buf.h
 *
 * @brief Dynamic buffer declarations
 *
 * @author Lars Wander
 */

#ifndef _DYN_BUF_H_
#define _DYN_BUF_H_

#define DYN_BUF_INIT_SIZE 0x10

/**
 * @brief recast a void func(type *elem) to void _func(void *elem)
 */
#define MAKE_VOID_FREE(n) void _##n(void *p) { n(p); }

struct _dyn_buf;
typedef struct _dyn_buf dyn_buf_t;

dyn_buf_t *dyn_buf_new();

int dyn_buf_push(dyn_buf_t *buf, void *elem);
int dyn_buf_at(dyn_buf_t *buf, int ind, void **elem);
int dyn_buf_len(dyn_buf_t *buf);
void dyn_buf_free(dyn_buf_t *dyn, void (*free_elem)(void *));

#endif /* _DYN_BUF_H_ */
