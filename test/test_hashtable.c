/**
 * @file test_hashtable.c
 *
 * @brief Unit tests for string -> 32bit int hastable
 *
 * @author Lars Wander
 */

#include "test_hashtable.h"
#include <lib/hashtable.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define EASY_KEY "ABCD"
#define EASY_VALUE 10203

int test_hashtable_easy() {
    htable_t *ht = htable_new();

    assert(htable_insert(ht, EASY_KEY, 3) >= 0);
    assert(htable_insert(ht, EASY_KEY, EASY_VALUE) >= 0);
    int v;
    assert(htable_lookup(ht, EASY_KEY, &v) >= 0);

    assert(v == EASY_VALUE);

    htable_free(ht, NULL);
    return 0;
}

#define HARD_ITERS 0x1000

int test_hashtable_hard() {
    htable_t *ht = htable_new();

    char key[100];

    /* Stuff the hashtable */
    for (int i = 0; i < HARD_ITERS; i++) {
        sprintf(key, "%d", i);
        assert(htable_insert(ht, key, i) >= 0);
    }

    /* Ensure it was stuffed correctly */
    for (int i = 0; i < HARD_ITERS; i++) {
        sprintf(key, "%d", i);
        int v;
        assert(htable_lookup(ht, key, &v) >= 0);
        assert(v == i);
    }

    /* Stuff the hashtable ... again */
    for (int i = 0; i < HARD_ITERS; i++) {
        sprintf(key, "%d", i);
        assert(htable_insert(ht, key, i * 2) >= 0);
    }

    /* Ensure it was stuffed correctly ... again */
    for (int i = 0; i < HARD_ITERS; i++) {
        sprintf(key, "%d", i);
        int v;
        assert(htable_lookup(ht, key, &v) >= 0);
        assert(v == i * 2);
    }

    /* Now delete the even entries */
    for (int i = 0; i < HARD_ITERS; i += 2) {
        sprintf(key, "%d", i);
        assert(htable_delete(ht, key, NULL) >= 0);
    }

    /* Make sure all even entries are gone */
    for (int i = 0; i < HARD_ITERS; i += 2) {
        sprintf(key, "%d", i);
        int v;
        assert(htable_lookup(ht, key, &v) < 0);
    }

    /*  Reinsert even entries */
    for (int i = 0; i < HARD_ITERS; i += 2) {
        sprintf(key, "%d", i);
        assert(htable_insert(ht, key, i * 2) >= 0);
    }

    /* Ensure it was stuffed correctly ... again */
    for (int i = 0; i < HARD_ITERS; i++) {
        sprintf(key, "%d", i);
        int v;
        assert(htable_lookup(ht, key, &v) >= 0);
        assert(v == i * 2);
    }

    htable_free(ht, NULL);
    return 0;
}
