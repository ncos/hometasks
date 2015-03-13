
// Local header files
#include "list.h"



int  lt_add_node (struct Head *list, struct Node *new_node);






void lt_rm_node(struct Head *list, struct Node *node)
{
    if (node->prev == NULL)
    {
        list->first = node->next;
        if(node->next != NULL)
            node->next->prev = NULL;
    }
    else
    {
        node->prev->next = node->next;
        if(node->next != NULL)
            node->next->prev = node->prev;
    }

    free(node);
};


void lt_destroy (struct Head *list)
{
    if(list == NULL)
        return;
    while(list->first != NULL)
        lt_rm_node(list, list->first);
};



struct Node* lt_get_first (struct Head *list)
{
    return list->first;
};


struct Node* lt_get_last  (struct Head *list)
{
    struct Node* it;
    if(list->first == NULL)
        return NULL;
    for (it = list->first; it->next != NULL; it = it->next);
    return it;
};


int lt_add_node(struct Head *list, struct Node *new_node)
{
    struct Node *last_node = lt_get_last(list);
    if(last_node == NULL)
    {
        new_node->prev = NULL;
        new_node->next = NULL;
        list->first = new_node;
    }
    else
    {
        last_node->next = new_node;
        new_node->prev = last_node;
        new_node->next = NULL;
    }

    errno = 0;
    return errno;
};


int lt_append(struct Head *list, char *key, char *value)
{
    if(list == NULL || key == NULL || value == NULL)
    {
        errno = EINVAL;
        return errno;
    }

    if(lt_search(list, key) != NULL)
    {
        errno = EIO;
        return errno;
    }

    struct Node *new_node;
    new_node = (struct Node*) calloc (1, sizeof(struct Node));
    if(new_node == NULL)
    {
        errno = ENOMEM;
        return errno;
    }

    //new_node->key = key;
    strcpy(new_node->key, key);
    new_node->value = value;
    return lt_add_node(list, new_node);
};



void lt_print_node(struct Head *list, struct Node *node, char *offset)
{
    printf("%s(%10p -> %10p): key = %s, value = %s\n", offset, node, node->next, node->key, node->value);
};

void lt_print_node_short(struct Head *list, struct Node *node, char *offset)
{
    printf("%s%s", offset, node->key);
};



struct Node* lt_search (struct Head *list, char *key)
{
    struct Node* result = NULL;
    struct Node* it;
    for (it = list->first; it != NULL; it = it->next)
    {
        if(strcmp(it->key, key) == 0)
        {
            result = it;
            break;
        }
    }

    return result;
};
