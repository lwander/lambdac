/**
 * @file dyn_buf_private.h
 *
 * @brief dyn buf data structure internals
 *
 * @author Lars Wander 
 */

#ifndef _DYN_BUF_PRIVATE_H_
#define _DYN_BUF_PRIVATE_H_

/**
 * Buffer that dynamically resizes in response to "push" operations
 */
typedef struct _dyn_buf {
    /* Contains an array of `void *` elements */
    void **buf;

    /* Largest possible capacity */
    int max_size;

    /* Next free element */
    int cur_size;
} dyn_buf_t;

#endif /* _DYN_BUF_PRIVATE_H_ */
