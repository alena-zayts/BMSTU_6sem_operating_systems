#include "check_main.h"

START_TEST(test_front_back_split_empty_list)
{
    node_t *head = NULL;
    node_t *back = NULL;

    front_back_split(head, &back);
    ck_assert_ptr_null(head);
    ck_assert_ptr_null(back);
}
END_TEST

START_TEST(test_front_back_split_1elem_in_list)
{
    data_t data = {"Test", 10};
    node_t node = {&data, NULL};
    node_t *head = &node;
    node_t *back = &node;

    front_back_split(head, &back);
    ck_assert_ptr_eq(head, &node);

    //connection cut
    ck_assert_ptr_null(node.next);

    ck_assert_ptr_null(back);
}
END_TEST

START_TEST(test_front_back_split_2elems_in_list)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};

    node_t node2 = {&data2, NULL};
    node_t node1 = {&data1, &node2};
    node_t *head = &node1;

    node_t *back = &node1;

    front_back_split(head, &back);
    //head
    ck_assert_ptr_eq(head, &node1);

    //connection cut
    ck_assert_ptr_null(node1.next);

    //back
    ck_assert_ptr_eq(back, &node2);
    ck_assert_ptr_null(node2.next);
}
END_TEST

START_TEST(test_front_back_split_odd_amount)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};
    data_t data3 = {"Test3", 3};

    node_t node3 = {&data3, NULL};
    node_t node2 = {&data2, &node3};
    node_t node1 = {&data1, &node2};
    node_t *head = &node1;

    node_t *back = &node1;

    front_back_split(head, &back);
    //head
    ck_assert_ptr_eq(head, &node1);
    head = head->next;
    ck_assert_ptr_eq(head, &node2);

    //connection cut
    ck_assert_ptr_null(head->next);

    //back
    ck_assert_ptr_eq(back, &node3);
    ck_assert_ptr_null(back->next);
}
END_TEST

START_TEST(test_front_back_split_even_amount)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};
    data_t data3 = {"Test3", 3};
    data_t data4 = {"Test4", 4};

    node_t node4 = {&data4, NULL};
    node_t node3 = {&data3, &node4};
    node_t node2 = {&data2, &node3};
    node_t node1 = {&data1, &node2};
    node_t *head = &node1;

    node_t *back = &node1;

    front_back_split(head, &back);

    //head
    ck_assert_ptr_eq(head, &node1);
    head = head->next;
    ck_assert_ptr_eq(head, &node2);

    //connection cut
    ck_assert_ptr_null(head->next);

    //back
    ck_assert_ptr_eq(back, &node3);
    back = back->next;
    ck_assert_ptr_eq(back, &node4);
    ck_assert_ptr_null(back->next);
}
END_TEST

Suite* front_back_split_suite(void)
{
    Suite *s;
    TCase *tc_neg;
    TCase *tc_pos;

    s = suite_create("front_back_split");

    tc_neg = tcase_create("negatives");
    tcase_add_test(tc_neg, test_front_back_split_empty_list);
    suite_add_tcase(s, tc_neg);

    tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, test_front_back_split_1elem_in_list);
    tcase_add_test(tc_pos, test_front_back_split_2elems_in_list);
    tcase_add_test(tc_pos, test_front_back_split_odd_amount);
    tcase_add_test(tc_pos, test_front_back_split_even_amount);
    suite_add_tcase(s, tc_pos);

    return s;
}

START_TEST(test_sorted_merge_both_emty)
{
    node_t *heada = NULL;
    node_t *headb = NULL;
    node_t *head_final = sorted_merge(&heada, &headb, my_comparator);

    ck_assert_ptr_null(heada);
    ck_assert_ptr_null(headb);
    ck_assert_ptr_null(head_final);
}
END_TEST

START_TEST(test_sorted_merge_a_emty)
{
    node_t *heada = NULL;

    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};
    data_t data3 = {"Test3", 3};

    node_t node3 = {&data3, NULL};
    node_t node2 = {&data2, &node3};
    node_t node1 = {&data1, &node2};
    node_t *headb = &node1;

    node_t *head_final = sorted_merge(&heada, &headb, my_comparator);

    ck_assert_ptr_null(heada);
    ck_assert_ptr_null(headb);
    ck_assert_ptr_nonnull(head_final);
    ck_assert_int_eq(nodes_eq(head_final, &node1), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node2), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node3), 1);
    head_final = head_final->next;
    ck_assert_ptr_null(head_final);
}
END_TEST

START_TEST(test_sorted_merge_b_emty)
{
    node_t *headb = NULL;

    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};
    data_t data3 = {"Test3", 3};

    node_t node3 = {&data3, NULL};
    node_t node2 = {&data2, &node3};
    node_t node1 = {&data1, &node2};
    node_t *heada = &node1;

    node_t *head_final = sorted_merge(&heada, &headb, my_comparator);

    ck_assert_ptr_null(heada);
    ck_assert_ptr_null(headb);
    ck_assert_ptr_nonnull(head_final);
    ck_assert_int_eq(nodes_eq(head_final, &node1), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node2), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node3), 1);
    head_final = head_final->next;
    ck_assert_ptr_null(head_final);
}
END_TEST

START_TEST(test_sorted_merge_firsta_thenb)
{
    data_t data1a = {"Test1", 1};
    data_t data2a = {"Test2", 2};
    data_t data3a = {"Test3", 3};

    node_t node3a = {&data3a, NULL};
    node_t node2a = {&data2a, &node3a};
    node_t node1a = {&data1a, &node2a};
    node_t *heada = &node1a;


    data_t data1b = {"Test3", 4};
    data_t data2b = {"Test4", 4};

    node_t node2b = {&data2b, NULL};
    node_t node1b = {&data1b, &node2b};
    node_t *headb = &node1b;


    node_t *head_final = sorted_merge(&heada, &headb, my_comparator);

    ck_assert_ptr_null(heada);
    ck_assert_ptr_null(headb);
    ck_assert_ptr_nonnull(head_final);
    ck_assert_int_eq(nodes_eq(head_final, &node1a), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node2a), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node3a), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node1b), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node2b), 1);
    head_final = head_final->next;
    ck_assert_ptr_null(head_final);
}
END_TEST

START_TEST(test_sorted_merge_mixed)
{
    data_t data1a = {"Test1", 1};
    data_t data2a = {"Test2", 2};
    data_t data3a = {"Test5", 5};

    node_t node3a = {&data3a, NULL};
    node_t node2a = {&data2a, &node3a};
    node_t node1a = {&data1a, &node2a};
    node_t *heada = &node1a;


    data_t data1b = {"Test1", 0};
    data_t data2b = {"Test3", 2};

    node_t node2b = {&data2b, NULL};
    node_t node1b = {&data1b, &node2b};
    node_t *headb = &node1b;


    node_t *head_final = sorted_merge(&heada, &headb, my_comparator);

    ck_assert_ptr_null(heada);
    ck_assert_ptr_null(headb);
    ck_assert_ptr_nonnull(head_final);
    ck_assert_int_eq(nodes_eq(head_final, &node1b), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node1a), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node2a), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node2b), 1);
    head_final = head_final->next;
    ck_assert_int_eq(nodes_eq(head_final, &node3a), 1);
    head_final = head_final->next;
    ck_assert_ptr_null(head_final);
}
END_TEST

Suite* sorted_merge_suite(void)
{
    Suite *s;
    TCase *tc_neg;
    TCase *tc_pos;

    s = suite_create("sorted_merge");

    tc_neg = tcase_create("negatives");
    tcase_add_test(tc_neg, test_sorted_merge_both_emty);
    suite_add_tcase(s, tc_neg);

    tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, test_sorted_merge_a_emty);
    tcase_add_test(tc_pos, test_sorted_merge_b_emty);
    tcase_add_test(tc_pos, test_sorted_merge_firsta_thenb);
    tcase_add_test(tc_pos, test_sorted_merge_mixed);
    suite_add_tcase(s, tc_pos);

    return s;
}

START_TEST(test_sort_empty_list)
{
    node_t *head = NULL;

    head = sort(head, my_comparator);
    ck_assert_ptr_null(head);
}
END_TEST

START_TEST(test_sort_1elem_in_list)
{
    data_t data = {"Test", 10};
    node_t node = {&data, NULL};
    node_t *head = &node;

    head = sort(head, my_comparator);

    ck_assert_ptr_eq(head, &node);

    ck_assert_ptr_null(head->next);
}
END_TEST

START_TEST(test_sort_all_same)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test1", 1};
    data_t data3 = {"Test1", 1};
    data_t data4 = {"Test1", 1};

    node_t node4 = {&data4, NULL};
    node_t node3 = {&data3, &node4};
    node_t node2 = {&data2, &node3};
    node_t node1 = {&data1, &node2};
    node_t *head = &node1;

    head = sort(head, my_comparator);

    ck_assert_ptr_nonnull(head);
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

START_TEST(test_sort_usual)
{
    data_t data1 = {"Test1", 10};
    data_t data2 = {"Test1", 1};
    data_t data3 = {"Test3", 1};
    data_t data4 = {"Test1", 1};

    node_t node4 = {&data4, NULL};
    node_t node3 = {&data3, &node4};
    node_t node2 = {&data2, &node3};
    node_t node1 = {&data1, &node2};
    node_t *head = &node1;

    head = sort(head, my_comparator);

    ck_assert_ptr_nonnull(head);
    ck_assert_int_eq(nodes_eq(head, &node2), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node4), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node3), 1);
    head = head->next;
    ck_assert_int_eq(nodes_eq(head, &node1), 1);
    head = head->next;
    ck_assert_ptr_null(head);
}
END_TEST

Suite* sort_suite(void)
{
    Suite *s;
    TCase *tc_neg;
    TCase *tc_pos;

    s = suite_create("sort");

    tc_neg = tcase_create("negatives");
    tcase_add_test(tc_neg, test_sort_empty_list);
    suite_add_tcase(s, tc_neg);

    tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, test_sort_1elem_in_list);
    tcase_add_test(tc_pos, test_sort_all_same);
    tcase_add_test(tc_pos, test_sort_usual);
    suite_add_tcase(s, tc_pos);

    return s;
}

