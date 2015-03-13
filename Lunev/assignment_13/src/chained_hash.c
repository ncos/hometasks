
#include "chained_hash.h"


struct Hash_table
{
    int checked_out;
    unsigned int size;
    struct Head *heads;
};


struct Iterator
{
    int empty;
    struct Hash_table *this_ht;
    unsigned int current_head;
    struct Node *current_node;
    struct Node *first_node;
    struct Node *last_node;
};


int ht_allocate_iterator(struct Iterator** it)
{
    errno = 0;

    (*it) = (struct Iterator*) calloc (1, sizeof(struct Iterator));
    if((*it) == NULL)
    {
        errno = ENOMEM;
        return errno;
    }

    return errno;
};





int ht_renew_iterator(struct Iterator *it, struct Hash_table *ht)
{
    errno = 0;
    if(it == NULL || ht == NULL)
    {
        errno = EINVAL;
        return errno;
    }

    it->this_ht = ht;
    it->first_node = ht_get_first(ht);
    it->last_node  = ht_get_last (ht);

    it->empty = 0;
    if(it->first_node == NULL || it->last_node == NULL)
        it->empty = 1;

    it->current_node = it->first_node;

    it->current_head = 0;

    if(!it->empty)
        while(ht->heads[it->current_head].first == NULL)
            it->current_head ++;

    ht->checked_out = 1;
    return errno;
};


struct Node *ht_get_last(struct Hash_table *ht)
{
    errno = 0;
    if(ht == NULL)
    {
        errno = EINVAL;
        return NULL;
    }

    int i = ht->size - 1;
    for(; i >= 0; i--)
    {
        if(ht->heads[i].first != NULL)
        {
            return lt_get_last(&(ht->heads[i]));
        }
    }

    return NULL;
};


struct Node *ht_get_first(struct Hash_table *ht)
{
    errno = 0;
    if(ht == NULL)
    {
        errno = EINVAL;
        return NULL;
    }

    int i = 0;
    for(; i < ht->size; i++)
    {
        if(ht->heads[i].first != NULL)
        {
            return lt_get_first(&(ht->heads[i]));
        }
    }

    return NULL;

};


int ht_allocate(struct Hash_table** ht, unsigned int size)
{
    errno = 0;
    if(size == 0)
    {
        (*ht) = NULL;
        errno = EINVAL;
        return errno;
    }

    (*ht) = (struct Hash_table*) calloc (1, sizeof(struct Hash_table));
    if((*ht) == NULL)
    {
        errno = ENOMEM;
        return errno;
    }



    (*ht)->size = size;
    (*ht)->heads = (struct Head*) calloc (size, sizeof(struct Head));
    if((*ht)->heads == NULL)
    {
        free(*ht);
        (*ht) = NULL;
        errno = ENOMEM;
        return errno;
    }

    (*ht)->checked_out = 0;
    return errno;
};


void ht_destroy(struct Hash_table *ht)
{
    if(ht == NULL)
        return;

    int i = 0;
    for(i = 0; i < ht->size; i++)
    {
        lt_destroy(&(ht->heads[i]));
    }
    free(ht->heads);
    free(ht);
};


int ht_append(struct Hash_table *ht, char *key, char *value)
{
    unsigned int head_id;
    head_id = ht_get_head_num(key, ht->size);
    if(lt_append(&(ht->heads[head_id]), key, value) == 0)
    {
        errno = 0;
        ht->checked_out = 0;
    }
    return errno;
};


int ht_remove(struct Hash_table *ht, char *key)
{
    unsigned int head_id;
    head_id = ht_get_head_num(key, ht->size);

    struct Node *node = lt_search (&(ht->heads[head_id]), key);
    if(node == NULL)
    {
        errno = EBADF;
        return errno;
    }

    lt_rm_node (&(ht->heads[head_id]), node);
    errno = 0;
    ht->checked_out = 0;
    return errno;
};


struct Node* ht_search (struct Hash_table *ht, char *key)
{
    unsigned int head_id;
    head_id = ht_get_head_num(key, ht->size);
    return lt_search (&(ht->heads[head_id]), key);
};


unsigned int ht_get_head_num(char *key, unsigned int max_size)
{
    unsigned long int hash = 0;
    int c;
    while ((c = *key++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash % max_size;
};


void ht_print(struct Hash_table *ht)
{
    int i = 0;
    for(; i < ht->size; i++)
    {
        printf("%d: ", i);

        struct Node* it;
        for (it = ht->heads[i].first; it != NULL; it = it->next)
        {
            lt_print_node_short(&(ht->heads[i]), it, "\t|\t");
        }
        printf("\n");
    }
};




int it_move_next(struct Iterator *it)
{
    errno = 0;
    if(it == NULL)
    {
        errno = EINVAL;
        return errno;
    }

    if(!it->this_ht->checked_out)
    {
        errno = EFAULT;
        return errno;
    }

    if(it->empty)
    {
        errno = ENODEV;
        return errno;
    }

    if(it_is_last(it))
    {
        errno = EDQUOT;
        return errno;
    }

    if(it->current_node->next != NULL)
        it->current_node = it->current_node->next;
    else
    {
        it->current_node = NULL;
        while (it->current_node == NULL)
        {
            it->current_head ++;
            it->current_node = lt_get_first (&(it->this_ht->heads[it->current_head]));
        }
    }
    return errno;
};


int it_move_prev(struct Iterator *it)
{
    errno = 0;
    if(it == NULL)
    {
        errno = EINVAL;
        return errno;
    }

    if(!it->this_ht->checked_out)
    {
        errno = EFAULT;
        return errno;
    }

    if(it->empty)
    {
        errno = ENODEV;
        return errno;
    }

    if(it_is_first(it))
    {
        errno = EDQUOT;
        return errno;
    }


    if(it->current_node->prev != NULL)
        it->current_node = it->current_node->prev;
    else
    {
        it->current_node = NULL;
        while (it->current_node == NULL)
        {
            it->current_head --;
            it->current_node = lt_get_last (&(it->this_ht->heads[it->current_head]));
        }
    }
    return errno;
};


int it_move_first(struct Iterator *it)
{
    errno = 0;
    if(it == NULL)
    {
        errno = EINVAL;
        return errno;
    }

    if(!it->this_ht->checked_out)
    {
        errno = EFAULT;
        return errno;
    }

    if(it->empty)
    {
        errno = ENODEV;
        return errno;
    }

    it->current_head = 0;
    while(it->this_ht->heads[it->current_head].first == NULL)
        it->current_head ++;

    it->current_node = it->first_node;
    return errno;
};


int it_move_last(struct Iterator *it)
{
    errno = 0;
    if(it == NULL)
    {
        errno = EINVAL;
        return errno;
    }

    if(!it->this_ht->checked_out)
    {
        errno = EFAULT;
        return errno;
    }

    if(it->empty)
    {
        errno = ENODEV;
        return errno;
    }

    it->current_head = it->this_ht->size - 1;
    while(it->this_ht->heads[it->current_head].first == NULL)
        it->current_head --;

    it->current_node = it->last_node;
    return errno;
};


int it_is_first(struct Iterator *it)
{
    if(it == NULL)
        return 0;
    if(it->first_node == it->current_node)
        return 1;
    return 0;
};


int it_is_last(struct Iterator *it)
{
    if(it == NULL)
        return 0;
    if(it->last_node == it->current_node)
        return 1;
    return 0;
};


struct Node *it_get_current (struct Iterator *it)
{
    return (it->current_node);
};
