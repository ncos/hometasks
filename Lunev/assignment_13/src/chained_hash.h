#ifndef CHAINED_HASH_H
#define CHAINED_HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#include "list.h"



struct Hash_table;

struct Iterator;


int ht_allocate(struct Hash_table** ht, unsigned int size);
int ht_allocate_iterator(struct Iterator** it);

int ht_renew_iterator(struct Iterator *it, struct Hash_table *ht);
int ht_append(struct Hash_table *ht, char *key, char *value);
int ht_remove(struct Hash_table *ht, char *key);
void ht_print(struct Hash_table *ht);

struct Node *ht_get_first(struct Hash_table *ht);
struct Node *ht_get_last (struct Hash_table *ht);
struct Node *ht_search (struct Hash_table *ht, char *key);


unsigned int ht_get_head_num(char *key, unsigned int max_size);
void ht_destroy(struct Hash_table *ht);


int it_move_next(struct Iterator *it);
int it_move_prev(struct Iterator *it);
int it_move_first(struct Iterator *it);
int it_move_last(struct Iterator *it);
int it_is_first (struct Iterator *it);
int it_is_last  (struct Iterator *it);
struct Node* it_get_current (struct Iterator *it);



#endif
