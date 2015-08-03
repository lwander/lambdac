/**
 * @file dyn_buf.c
 *
 * @brief Dynamic buffer implementation
 *
 * @author Lars Wander
 */

#include <lib/dyn_buf.h>
#include <err.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dyn_buf_private.h"

/**
 * @brief Creates a new dynamic buffer
 */
dyn_buf_t *dyn_buf_new() {
    dyn_buf_t *res;
    if ((res = calloc(1, sizeof(dyn_buf_t))) == NULL)
        goto cleanup_default;

    if ((res->buf = calloc(DYN_BUF_INIT_SIZE, sizeof(void *))) == NULL)
        goto cleanup_dyn_buf;

    res->max_size = DYN_BUF_INIT_SIZE;

    return res;

cleanup_dyn_buf:
    free(res);

cleanup_default:
    return NULL;
}

/**
 * @brief Double the dynamic buffer size & copy into a new buffer
 *
 * @param dyn The dynamic buffer to be grown
 *
 * @return 0 on success, ERR_* otherwise
 */
int _dyn_buf_grow(dyn_buf_t *dyn) {
    /* Double array at each growth operation */
    void **new_buf;
    if ((new_buf = malloc(dyn->max_size * 2 * sizeof(void *))) == NULL)
        return ERR_MEM_ALLOC;

    memcpy(new_buf, dyn->buf, dyn->max_size);
    free(dyn->buf);
    dyn->buf = new_buf;
    dyn->max_size *= 2;
    return 0;
}

/**
 * @brief Push an element into the buffer, resizing if needbe
 *
 * @param dyn Buffer inserted into
 * @param elem element begin inserted
 *
 * @return 0 on success, ERR_* otherwise
 */
int dyn_buf_push(dyn_buf_t *dyn, void *elem) {
    int res;
    if (dyn == NULL) {
        res = ERR_INP;
        goto cleanup_default;
    }

    assert(dyn->cur_size <= dyn->max_size);

    if (dyn->cur_size == dyn->max_size)
        if ((res = _dyn_buf_grow(dyn)) < 0)
            goto cleanup_default;

    dyn->buf[dyn->cur_size] = elem;
    dyn->cur_size++;

    return 0;

cleanup_default:
    return res;
}

/**
 * @brief Access element at specified index. Fails if OOB
 *
 * @param dyn Buffer being examined
 * @param ind Index of element being searched
 * @param elem On success, required element may be placed here
 *
 * @return 0 on success, ERR_* otherwise
 */
int dyn_buf_at(dyn_buf_t *dyn, int ind, void **elem) {
    if (dyn == NULL || elem == NULL)
        return ERR_INP;

    if (dyn->cur_size >= ind)
        return ERR_OOB;

    *elem = dyn->buf[ind];
    return 0;
}

/**
 * @brief Delete a dynamic buffer
 *
 * @param dyn The dynamic buffer to be freed
 * @param free_elem A custom function for deleting the dyn_buf elements.
 *        NULL if not needed
 */
void dyn_buf_free(dyn_buf_t *dyn, void (*free_elem)(void *)) {
    if (free_elem != NULL)
        for (int i = 0; i < dyn->cur_size; i++)
            (*free_elem)(dyn->buf[i]);

    free(dyn->buf);
    free(dyn);
}
