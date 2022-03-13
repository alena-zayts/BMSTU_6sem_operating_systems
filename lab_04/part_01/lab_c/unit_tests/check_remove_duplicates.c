#include "check_main.h"

START_TEST(test_remove_duplicates_empty_ptr)
{
    node_t **head = NULL;

    remove_duplicates(head, my_comparator);
    ck_assert_ptr_null(head);
}
END_TEST

START_TEST(test_remove_duplicates_empty_list)
{
    node_t *head = NULL;

    remove_duplicates(&head, my_comparator);
    ck_assert_ptr_null(head);
}
END_TEST

START_TEST(test_remove_duplicates_1elem_in_list)
{
    data_t data1 = {"Test1", 1};
    node_t *node1 = malloc(sizeof(node_t));
    if (node1)
    {
        node1->data = &data1;
        node1->next = NULL;

        node_t *head = node1;

        remove_duplicates(&head, my_comparator);
        ck_assert_ptr_nonnull(head);
        ck_assert_int_eq(nodes_eq(head, node1), 1);
        head = head->next;
        ck_assert_ptr_null(head);
        free(node1);
    }
}
END_TEST

START_TEST(test_remove_2same_elems_in_list)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test1", 1};

    node_t *node2 = malloc(sizeof(node_t));
    if (node2)
    {
        node2->data = &data2;
        node2->next = NULL;
        node_t *node1 = malloc(sizeof(node_t));
        if (node1)
        {
            node1->data = &data1;
            node1->next = node2;

            node_t *head = node1;

            remove_duplicates(&head, my_comparator);
            ck_assert_ptr_nonnull(head);
            ck_assert_int_eq(nodes_eq(head, node1), 1);
            head = head->next;
            ck_assert_ptr_null(head);
        }
        free(node1);
    }
}
END_TEST

START_TEST(test_remove_duplicates_no_duplicates)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 1};
    data_t data3 = {"Test3", 3};
    data_t data4 = {"Test3", 4};

    node_t *node4 = malloc(sizeof(node_t));
    if (!node4)
        return;
    node4->data = &data4;
    node4->next = NULL;
    node_t *node3 = malloc(sizeof(node_t));
    if (!node3)
    {
        free(node4);
        return;
    }
    node3->data = &data3;
    node3->next = node4;
    node_t *node2 = malloc(sizeof(node_t));
    if (!node2)
    {
        free(node4);
        free(node3);
        return;
    }
    node2->data = &data2;
    node2->next = node3;
    node_t *node1 = malloc(sizeof(node_t));
    if (!node1)
    {
        free(node4);
        free(node3);
        free(node2);
        return;
    }
    node1->data = &data1;
    node1->next = node2;

    node_t *head = node1;

    remove_duplicates(&head, my_comparator);
    ck_assert_ptr_nonnull(head);
    ck_assert_int_eq(nodes_eq(head, node1), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, node2), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, node3), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, node4), 1);
    head = head->next;
    ck_assert_ptr_null(head);
    free(node1);
    free(node2);
    free(node3);
    free(node4);
}
END_TEST

START_TEST(test_remove_duplicates_usual)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test1", 1};
    //
    data_t data3 = {"Test3", 3};
    //
    data_t data4 = {"Test3", 4};
    data_t data5 = {"Test3", 4};
    data_t data6 = {"Test3", 4};
    //
    data_t data7 = {"Test7", 4};
    data_t data8 = {"Test7", 4};

    node_t *node8 = malloc(sizeof(node_t));
    if (!node8)
        return;
    node8->data = &data8;
    node8->next = NULL;
    node_t *node7 = malloc(sizeof(node_t));
    if (!node7)
    {
        free(node8);
        return;
    }
    node7->data = &data7;
    node7->next = node8;
    node_t *node6 = malloc(sizeof(node_t));
    if (!node6)
    {
        free(node7);
        free(node8);
        return;
    }
    node6->data = &data6;
    node6->next = node7;
    node_t *node5 = malloc(sizeof(node_t));
    if (!node5)
    {
        free(node6);
        free(node7);
        free(node8);
        return;
    }
    node5->data = &data5;
    node5->next = node6;
    node_t *node4 = malloc(sizeof(node_t));
    if (!node4)
    {
        free(node5);
        free(node6);
        free(node7);
        free(node8);
        return;
    }
    node4->data = &data4;
    node4->next = node5;
    node_t *node3 = malloc(sizeof(node_t));
    if (!node3)
    {
        free(node4);
        free(node5);
        free(node6);
        free(node7);
        free(node8);
        return;
    }
    node3->data = &data3;
    node3->next = node4;
    node_t *node2 = malloc(sizeof(node_t));
    if (!node2)
    {
        free(node3);
        free(node4);
        free(node5);
        free(node6);
        free(node7);
        free(node8);
        return;
    }
    node2->data = &data2;
    node2->next = node3;
    node_t *node1 = malloc(sizeof(node_t));
    if (!node1)
    {
        free(node2);
        free(node3);
        free(node4);
        free(node5);
        free(node6);
        free(node7);
        free(node8);
        return;
    }
    node1->data = &data1;
    node1->next = node2;
    node_t *head = node1;

    remove_duplicates(&head, my_comparator);
    ck_assert_ptr_nonnull(head);

    ck_assert_int_eq(nodes_eq(head, node1), 1);
    head = head->next;

    //no 2

    ck_assert_int_eq(nodes_eq(head, node3), 1);
    head = head->next;

    ck_assert_int_eq(nodes_eq(head, node4), 1);
    head = head->next;

    //no 5, 6

    ck_assert_int_eq(nodes_eq(head, node7), 1);
    head = head->next;

    //no 8

    ck_assert_ptr_null(head);
    free(node1);
    free(node3);
    free(node4);
    free(node7);
}
END_TEST

Suite* remove_duplicates_suite(void)
{
    Suite *s;
    TCase *tc_neg;
    TCase *tc_pos;

    s = suite_create("remove_duplicates");

    tc_neg = tcase_create("negatives");
    tcase_add_test(tc_neg, test_remove_duplicates_empty_list);
    tcase_add_test(tc_neg, test_remove_duplicates_empty_ptr);
    suite_add_tcase(s, tc_neg);

    tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, test_remove_duplicates_1elem_in_list);
    tcase_add_test(tc_pos, test_remove_2same_elems_in_list);
    tcase_add_test(tc_pos, test_remove_duplicates_no_duplicates);
    tcase_add_test(tc_pos, test_remove_duplicates_usual);
    suite_add_tcase(s, tc_pos);

    return s;
}