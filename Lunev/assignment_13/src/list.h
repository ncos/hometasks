#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>


struct Node
{
    struct Node *next;
    struct Node *prev;
    //char *key;
    char key[40];
    char *value;
};

struct Head
{
    struct Node *first;
};



void lt_rm_node (struct Head *list, struct Node *node     );
void lt_destroy (struct Head *list);
int  lt_append(struct Head *list, char *key, char *value  );
void lt_print_node(struct Head *list, struct Node *node, char *offset);
void lt_print_node_short(struct Head *list, struct Node *node, char *offset);


struct Node* lt_search (struct Head *list, char *key);
struct Node* lt_get_first (struct Head *list);
struct Node* lt_get_last  (struct Head *list);




#endif
