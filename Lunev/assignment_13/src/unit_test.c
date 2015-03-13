
// C standard includes
#include <stdio.h>
#include <stdlib.h>

// Local header files
#include "chained_hash.h"

// for evaluation run:
// gcov --object-directory obj src/list.c;
// gcov --object-directory obj src/chained_hash.c
// valgrind ./chained_hash_test

int main()
{
    printf("Testing...\n\n\n");


    struct Hash_table *ht;
    struct Hash_table *ht_empty;


    printf("ht_allocate test:\n");
    if(ht_allocate(&ht,  0) != 0) perror("ERROR: ht_allocate(&ht,  0) : ");
    ht_destroy(ht);
    if(ht_allocate(&ht, 10) != 0) perror("ERROR: ht_allocate(&ht, 10) : ");
    ht_destroy(ht);
    if(ht_allocate(&ht, 20) != 0) perror("ERROR: ht_allocate(&ht, 20) : ");
    if(ht_allocate(&ht_empty, 8) != 0) perror("ERROR: hht_allocate(&ht_empty, 8) : ");
    if(errno == ENOMEM) exit(-1);

    printf("\n\nht_append test:\n");
    if(ht_append(ht, "soeme_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "soeme_key", "some_value") != 0) perror("ERROR: This key is already in list \t SUCCESS");
    if(ht_append(ht, "soew46yhwhme_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "soew6907me_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "soeq@$$$^h4me_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "soemogfe_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "soeTGS#Roeme_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "soeme_2key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "so*fk7&eme_kGey", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "soem!@ESe_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "so9feme_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "soeme_kN(&ey", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "s<BIVRx6oeme_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "so?JKFKUVf7eme_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "*6rcukuld", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "soemB^&I$e_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "soeY=$C!me_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    if(ht_append(ht, "so+_(GB?eme;_key", "some_value") != 0) perror("ERROR: This key is already in list \t FAILED");
    ht_print(ht);



    printf("\n\nht_search test:\n");
    struct Node* node;

    node = ht_search(ht, "none");
    if(node != NULL) lt_print_node(NULL, node, "  ");
    else printf("SUCCESS\n");

    node = ht_search(ht, "soemB^&I$e_key");
    if(node != NULL) lt_print_node(NULL, node, "  ");
    else printf("FAILED\n");

    node = ht_search(ht, "soeq@$$$^h4me_key");
    if(node != NULL) lt_print_node(NULL, node, "  ");
    else printf("FAILED\n");

    node = ht_search(ht, "soeme_key");
    if(node != NULL) lt_print_node(NULL, node, "  ");
    else printf("FAILED\n");


    printf("\n\nht_get_first/last test:\n");
    lt_print_node(NULL, ht_get_first(ht), "first - ");
    lt_print_node(NULL, ht_get_last (ht), "last - ");
    if(ht_get_first(NULL) != NULL) perror("ht_get_first: FAILED");
    if(ht_get_last (NULL) != NULL) perror("ht_get_last:  FAILED" );


    printf("\n\nht_remove test:\n");
    if(ht_remove(ht, "soeme_key") != 0) perror("ht_remove: FAILED"); errno = 0;
    if(ht_remove(ht, "soeme_key") != 0) perror("ht_remove: SUCCESS"); errno = 0;
    if(ht_remove(ht, "so9feme_key") != 0) perror("ht_remove: FAILED"); errno = 0;
    if(ht_remove(ht, "soeme_2key") != 0) perror("ht_remove: FAILED"); errno = 0;
    if(ht_remove(ht, "soemogfe_key") != 0) perror("ht_remove: FAILED"); errno = 0;
    if(ht_remove(ht, "so?JKFKUVf7eme_key") != 0) perror("ht_remove: FAILED"); errno = 0;
    if(ht_remove(ht, "soew6907me_key") != 0) perror("ht_remove: FAILED"); errno = 0;
    if(ht_remove(ht, "s<BIVRx6oeme_key") != 0) perror("ht_remove: FAILED"); errno = 0;

    ht_print(ht);



    printf("\n\niterator test:\n");
    struct Iterator *it;
    struct Iterator *it_empty;




    if(ht_allocate_iterator(&it) != 0) perror("ht_allocate_iterator: Failed full");
    if(ht_allocate_iterator(&it_empty) != 0) perror("ht_allocate_iterator: Failed empty");







    printf("\n\nht_renew_iterator test:\n");
    if(ht_renew_iterator(NULL, ht)  != 0) perror("ht_renew_iterator: it = NULL!");
    if(ht_renew_iterator(it, NULL) != 0) perror("ht_renew_iterator: ht = NULL!");
    if(ht_renew_iterator(it, ht)   != 0) perror("ht_renew_iterator: should not be there");
    if(ht_renew_iterator(it_empty, ht_empty)   != 0) perror("ht_renew_iterator: should not be there (empty ht)");



    printf("\n\nit_move_next/prev test:\n");
    if(it_move_first(it) != 0) perror("it_move_first: should not be there");

    while(it_move_next(it) == 0);
    perror("it_move_next: it overflow!");
    if(it_move_next(NULL) != 0) perror("it_move_next: it = NULL!");
    if(it_move_next(it_empty) != 0) perror("it_move_next: ht is empty!");
    if(ht_append(ht, "so+_(GB?efme;_key", "some_value") != 0) perror("ERROR: This key is already in list");
    if(it_move_next(it) != 0) perror("it_move_next: modified!");
    if(ht_renew_iterator(it, ht) != 0) perror("ht_renew_iterator: should not be there");



    if(it_move_last(it) != 0) perror("it_move_last: should not be there");

    while(it_move_prev(it) == 0);

    perror("it_move_prev: it underflow!");
    if(it_move_prev(NULL) != 0) perror("it_move_prev: it = NULL!");
    if(it_move_prev(it_empty) != 0) perror("it_move_prev: ht is empty!");
    if(ht_remove(ht, "so+_(GB?efme;_key") != 0) perror("ht_remove: cannot remove, not in hash table"); errno = 0;
    if(it_move_prev(it) != 0) perror("it_move_prev: modified!");
    if(ht_renew_iterator(it, ht)   != 0) perror("ht_renew_iterator: should not be there");


    printf("\n\nht_get_first/last test:\n");
    if(ht_append(ht, "so+_(GB?efme;_key", "some_value") != 0) perror("ERROR: This key is already in list");
    if(it_move_first(it) != 0) perror("it_move_first: modified!");
    if(ht_renew_iterator(it, ht)   != 0) perror("ht_renew_iterator: should not be there");

    if(it_move_first(NULL) != 0) perror("it_move_first: it = NULL");
    if(it_move_first(it_empty) != 0) perror("it_move_first: ht is empty!");
    if(it_move_first(it) != 0) perror("it_move_first: should not be there");
    //lt_print_node(NULL, it->current_node, "first - ");
    printf("it_is_first(it) = %d\n", it_is_first(it));
    printf("it_is_last(it) = %d\n", it_is_last(it));

    printf("\n");

    if(ht_remove(ht, "so+_(GB?efme;_key") != 0) perror("ht_remove: cannot remove, not in hash table"); errno = 0;
    if(it_move_last(it) != 0) perror("it_move_last: modified!");
    if(ht_renew_iterator(it, ht)   != 0) perror("ht_renew_iterator: should not be there");

    if(it_move_last(NULL) != 0) perror("it_move_last: it = NULL");
    if(it_move_last(it_empty) != 0) perror("it_move_last: ht is empty!");
    if(it_move_last(it) != 0) perror("it_move_last: should not be there");
    //lt_print_node(NULL, it->current_node, "last - ");
    printf("it_is_first(it) = %d\n", it_is_first(it));
    printf("it_is_last(it) = %d\n", it_is_last(it));

    printf("\n");
    printf("it_is_first(NULL) = %d\n", it_is_first(NULL));
    printf("it_is_last(NULL) = %d\n", it_is_last(NULL));

    lt_print_node(NULL, it_get_current (it), "current - ");
    errno = 0;


    printf("memory test");
    int i = 0;
    for(i = 0; i < 10000; i++)
    {
        char buffer [50];
        sprintf (buffer, "memtest-key-%d", i);

        //printf("%d\n", i);
        if(ht_append(ht, buffer, "some_value") == EIO) perror("ERROR: This key is already in list \t FAILED");
        if(errno == ENOMEM)
        {
            printf("Cannot append - no memory\n");

            ht_destroy(ht);
            ht_destroy(ht_empty);
            free(it);
            free(it_empty);

            exit(-1);
        }
    }

    ht_print(ht);




    printf("\n\nDestructing...\n");
    ht_destroy(ht);
    ht_destroy(ht_empty);
    free(it);
    free(it_empty);

	return 0;
};
