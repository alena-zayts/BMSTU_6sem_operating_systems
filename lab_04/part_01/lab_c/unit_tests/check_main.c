#include "check_main.h"

int nodes_eq(node_t *x, node_t *y)
{
    if ((x->next == y->next) && (x->data == y->data))
        return 1;
    else
        return 0;
}

int main(void)
{
    int no_failed = 0;
    SRunner *sr;

    //functions checked in test system
    sr = srunner_create(pop_front_suite());
    srunner_add_suite(sr, insert_suite());
    srunner_add_suite(sr, remove_duplicates_suite());
    srunner_add_suite(sr, front_back_split_suite());
    srunner_add_suite(sr, sorted_merge_suite());
    srunner_add_suite(sr, sort_suite());

    // //functions from task
    srunner_add_suite(sr, my_comparator_suite());
    srunner_add_suite(sr, my_getdelim_suite());
    srunner_add_suite(sr, task_create_node_suite()); //also uses task_free()
    srunner_add_suite(sr, task_read_suite());
    srunner_add_suite(sr, task_print_suite());

    //functions from task_list
    srunner_add_suite(sr, task_list_read_suite()); //also uses task_list_free()
    srunner_add_suite(sr, task_list_insert_sorted_suite());
    srunner_add_suite(sr, create_deletion_list_suite()); //also uses free_deletion_list()

    srunner_run_all(sr, CK_VERBOSE);
    no_failed = srunner_ntests_failed(sr);

    srunner_free(sr);

    return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
