/**
 * @file test.c
 *
 * @brief "main" file for running unit tests
 *
 * @author Lars Wander
 */

#include "test_hashtable.h"

#include <stdio.h>

int main() {
    printf("< HASHTABLE TEST >\n");
    printf("< EASY MODE... ");
    test_hashtable_easy();
    printf("PASSED >\n");
    printf("< HARD MODE... ");
    fflush(stdout);
    test_hashtable_hard();
    printf("PASSED >\n");
    return 0;
}
