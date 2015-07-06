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

#define MAKE_VOID_FREE(n) void _##n(void *p) { n(p); }

struct _dyn_buf;
typedef struct _dyn_buf dyn_buf_t;

dyn_buf_t *new_dyn_buf();

int dyn_buf_push(dyn_buf_t *buf, void *elem);
int dyn_buf_at(dyn_buf_t *buf, int ind, void **elem);
void free_dyn_buf(dyn_buf_t *dyn, void (*free_elem)(void *));

#endif /* _DYN_BUF_H_ */
