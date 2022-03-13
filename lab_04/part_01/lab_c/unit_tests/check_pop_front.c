#include "check_main.h"

START_TEST(test_pop_front_empty_ptr)
{
    node_t **head = NULL;
    void *data = NULL;

    data = pop_front(head);
    ck_assert_ptr_null(head);
    ck_assert_ptr_null(data);
}
END_TEST

START_TEST(test_pop_front_empty_list)
{
    node_t *head = NULL;
    void *data = NULL;

    data = pop_front(&head);
    ck_assert_ptr_null(head);
    ck_assert_ptr_null(data);
}
END_TEST

START_TEST(test_pop_front_1null_elem_list)
{
    void *data = NULL;
    node_t *node = malloc(sizeof(node_t));
    node->data = NULL;
    node->next = NULL;
    node_t *head = node;

    data = pop_front(&head);
    ck_assert_ptr_null(head);
    ck_assert_ptr_null(data);
}
END_TEST


START_TEST(test_pop_front_1nonnull_elem_list)
{
    data_t data_src = {"Test", 1};
    void *data_got = NULL;
    node_t *node = malloc(sizeof(node_t));
    node->data = &data_src;
    node->next = NULL;
    node_t *head = node;

    data_got = pop_front(&head);
    ck_assert_ptr_null(head);
    ck_assert_int_eq(((data_t *)data_got)->diff, 1);
    ck_assert_int_eq(strcmp(((data_t *)data_got)->name, "Test"), 0);
}
END_TEST

START_TEST(test_pop_front_usual_list)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};
    data_t data3 = {"Test3", 3};


    void *data_got = NULL;
    node_t *node3 = malloc(sizeof(node_t));
    node3->data = &data3;
    node3->next = NULL;
    node_t *node2 = malloc(sizeof(node_t));
    node2->data = &data2;
    node2->next = node3;
    node_t *node1 = malloc(sizeof(node_t));
    node1->data = &data1;
    node1->next = node2;

    node_t *head = node1;

    //first pop
    data_got = pop_front(&head);
    //got correct new_head
    ck_assert_ptr_eq(head, node2);
    ck_assert_ptr_eq(head->next, node3);
    //got correct data
    ck_assert_int_eq(((data_t *)data_got)->diff, 1);
    ck_assert_int_eq(strcmp(((data_t *)data_got)->name, "Test1"), 0);

    //second pop
    data_got = pop_front(&head);
    //got correct new_head
    ck_assert_ptr_eq(head, node3);
    ck_assert_ptr_eq(head->next, NULL);
    //got correct data
    ck_assert_int_eq(((data_t *)data_got)->diff, 2);
    ck_assert_int_eq(strcmp(((data_t *)data_got)->name, "Test2"), 0);

    //third pop
    data_got = pop_front(&head);
    //got correct new_head
    ck_assert_ptr_null(head);
    //got correct data
    ck_assert_int_eq(((data_t *)data_got)->diff, 3);
    ck_assert_int_eq(strcmp(((data_t *)data_got)->name, "Test3"), 0);
}
END_TEST

Suite* pop_front_suite(void)
{
    Suite *s;
    TCase *tc_pos;
    TCase *tc_neg;

    s = suite_create("pop_front");

    tc_neg = tcase_create("negatives");
    tcase_add_test(tc_neg, test_pop_front_empty_list);
    tcase_add_test(tc_neg, test_pop_front_1null_elem_list);
    tcase_add_test(tc_neg, test_pop_front_empty_ptr);
    suite_add_tcase(s, tc_neg);

    tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, test_pop_front_1nonnull_elem_list);
    tcase_add_test(tc_pos, test_pop_front_usual_list);
    suite_add_tcase(s, tc_pos);

    return s;
}
