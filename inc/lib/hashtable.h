/**
 * @file hashtable.h
 *
 * @brief Hash table definition
 *
 * Hashes strings to 32 bit values 
 *
 * @author Lars Wander
 */ 

#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

struct _htable;
typedef struct htable htable_t;

htable_t *htable_new();
int htable_insert(htable_t *ht, char *key, int value);
int htable_lookup(htable_t *ht, char *key, int *value);
int htable_delete(htable_t *ht, char *key, int *value);
void htable_free(htable_t *ht, void (*free_value)(int));

#endif /* _HASH_TABLE_H_ */
