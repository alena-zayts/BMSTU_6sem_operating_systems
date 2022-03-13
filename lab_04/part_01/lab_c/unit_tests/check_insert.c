#include "check_main.h"

START_TEST(test_insert_empty_list)
{
    data_t data_elem = {"Test1", 1};
    data_t data_before = {"Test2", 2};

    node_t elem = {&data_elem, NULL};
    node_t before = {&data_before, NULL};
    node_t *head = NULL;

    insert(&head, &elem, &before);
    //nothing has changed
    ck_assert_ptr_null(head);
    ck_assert_ptr_eq(before.data, &data_before);
    ck_assert_ptr_null(before.next);
    ck_assert_ptr_eq(elem.data, &data_elem);
    ck_assert_ptr_null(elem.next);
}
END_TEST

START_TEST(test_insert_empty_elem)
{
    data_t data_node = {"Test1", 1};
    data_t data_before = {"Test2", 2};

    node_t before = {&data_before, NULL};
    node_t node = {&data_node, &before};
    node_t *head = &node;
    node_t *elem = NULL;

    insert(&head, elem, &before);
    //nothing has changed
    ck_assert_ptr_eq(head, &node);
    ck_assert_ptr_eq(head->data, &data_node);
    ck_assert_ptr_eq(head->next, &before);
    ck_assert_ptr_eq(before.data, &data_before);
    ck_assert_ptr_null(before.next);
    ck_assert_ptr_null(elem);
}
END_TEST

START_TEST(test_insert_incorrect_elem)
{
    data_t data_node = {"Test1", 1};
    data_t data_before = {"Test2", 2};
    data_t data_elem = {"Test3", 3};

    node_t before = {&data_before, NULL};
    node_t node = {&data_node, &before};
    //elem has "next" elem
    node_t elem = {&data_elem, &before};
    node_t *head = &node;

    insert(&head, &elem, &before);
    //nothing has changed
    ck_assert_int_eq(nodes_eq(head, &node), 1);
    ck_assert_int_eq(nodes_eq(head->next, &before), 1);
    ck_assert_ptr_eq(elem.data, &data_elem);
    ck_assert_ptr_eq(elem.next, &before);
}
END_TEST

START_TEST(test_insert_before_not_found)
{
    data_t data_node1 = {"Test1", 1};
    data_t data_node2 = {"Test2", 2};
    data_t data_before = {"Test3", 3};
    data_t data_elem = {"Test4", 4};

    node_t before = {&data_before, NULL};
    node_t node2 = {&data_node2, NULL};
    node_t node1 = {&data_node1, &node2};
    node_t elem = {&data_elem, NULL};
    node_t *head = &node1;

    insert(&head, &elem, &before);
    //nothing has changed
    ck_assert_int_eq(nodes_eq(head, &node1), 1);
    ck_assert_int_eq(nodes_eq(head->next, &node2), 1);
    ck_assert_ptr_null((head->next)->next);
    ck_assert_ptr_eq(elem.data, &data_elem);
    ck_assert_ptr_null(elem.next);
}
END_TEST

START_TEST(test_insert_in_beginning)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};
    data_t data3 = {"Test3", 3};
    data_t data4 = {"Test4", 4};
    data_t data_elem = {"Test5", 5};

    node_t node4 = {&data4, NULL};
    node_t node3 = {&data3, &node4};
    node_t node2 = {&data2, &node3};
    node_t node1 = {&data1, &node2};
    node_t elem = {&data_elem, NULL};
    node_t *head = &node1;

    insert(&head, &elem, &node1);

    ck_assert_ptr_eq(head->next, &node1);
    ck_assert_ptr_eq(head->data, &data_elem);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node1), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node2), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node3), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node4), 1);
    head = head->next;
    ck_assert_ptr_null(head);
}
END_TEST

START_TEST(test_insert_in_middle)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};
    data_t data3 = {"Test3", 3};
    data_t data4 = {"Test4", 4};
    data_t data_elem = {"Test5", 5};

    node_t node4 = {&data4, NULL};
    node_t node3 = {&data3, &node4};
    node_t node2 = {&data2, &node3};
    node_t node1 = {&data1, &node2};
    node_t elem = {&data_elem, NULL};
    node_t *head = &node1;

    insert(&head, &elem, &node3);

    ck_assert_int_eq(nodes_eq(head, &node1), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node2), 1);
    head = head->next;
    ck_assert_ptr_eq(head->next, &node3);
    ck_assert_ptr_eq(head->data, &data_elem);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node3), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node4), 1);
    head = head->next;
    ck_assert_ptr_null(head);
}
END_TEST

START_TEST(test_insert_in_end)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};
    data_t data3 = {"Test3", 3};
    data_t data4 = {"Test4", 4};
    data_t data_elem = {"Test5", 5};

    node_t node4 = {&data4, NULL};
    node_t node3 = {&data3, &node4};
    node_t node2 = {&data2, &node3};
    node_t node1 = {&data1, &node2};
    node_t elem = {&data_elem, NULL};
    node_t *head = &node1;

    insert(&head, &elem, NULL);

    ck_assert_int_eq(nodes_eq(head, &node1), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node2), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node3), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node4), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &elem), 1);
    head = head->next;
    ck_assert_ptr_null(head);
}
END_TEST

Suite* insert_suite(void)
{
    Suite *s;
    TCase *tc_neg;
    TCase *tc_pos;

    s = suite_create("insert");

    tc_neg = tcase_create("negatives");
    tcase_add_test(tc_neg, test_insert_empty_list);
    tcase_add_test(tc_neg, test_insert_empty_elem);
    tcase_add_test(tc_neg, test_insert_incorrect_elem);
    tcase_add_test(tc_neg, test_insert_before_not_found);
    //if data in elem is null - is a mistake?
    suite_add_tcase(s, tc_neg);

    tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, test_insert_in_beginning);
    tcase_add_test(tc_pos, test_insert_in_middle);
    tcase_add_test(tc_pos, test_insert_in_end);
    suite_add_tcase(s, tc_pos);

    return s;
}
