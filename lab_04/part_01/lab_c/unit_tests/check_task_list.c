#include "check_main.h"

START_TEST(test_task_list_read_empty_file)
{
    node_t *head = NULL;

    FILE *f = fopen("unit_tests\\test_task_list_read_empty_file.txt", "r");
    if (f)
    {
        head = task_list_read(f);
        ck_assert_ptr_null(head);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_list_read_empty_name_beg)
{
    node_t *head = NULL;

    FILE *f = fopen("unit_tests\\test_task_list_read_empty_name_beg.txt", "r");
    if (f)
    {
        head = task_list_read(f);
        ck_assert_ptr_null(head);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_list_read_empty_diff_mid)
{
    node_t *head = NULL;

    FILE *f = fopen("unit_tests\\test_task_list_read_empty_diff_mid.txt", "r");
    if (f)
    {
        head = task_list_read(f);
        ck_assert_ptr_null(head);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_list_read_letter_diff_end)
{
    node_t *head = NULL;

    FILE *f = fopen("unit_tests\\test_task_list_read_letter_diff_end.txt", "r");
    if (f)
    {
        head = task_list_read(f);
        ck_assert_ptr_null(head);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_list_read_neg_diff_mid)
{
    node_t *head = NULL;

    FILE *f = fopen("unit_tests\\test_task_list_read_neg_diff_mid.txt", "r");
    if (f)
    {
        head = task_list_read(f);
        ck_assert_ptr_null(head);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_list_read_float_diff_beg)
{
    node_t *head = NULL;

    FILE *f = fopen("unit_tests\\test_task_list_read_float_diff_beg.txt", "r");
    if (f)
    {
        head = task_list_read(f);
        ck_assert_ptr_null(head);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_list_read_1task)
{
    data_t data1 = {"Test", 1};
    node_t node1 = {&data1, NULL};

    node_t *head2 = NULL;

    FILE *f = fopen("unit_tests\\test_task_list_read_1task.txt", "r");
    if (f)
    {
        head2 = task_list_read(f);
        ck_assert_ptr_nonnull(head2);
        ck_assert_int_eq(my_comparator(node1.data, head2->data), 0);
        ck_assert_ptr_null(head2->next);
        task_list_free(head2);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_list_read_usual)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};
    data_t data3 = {"Test3", 3};

    node_t *head = NULL;

    FILE *f = fopen("unit_tests\\test_task_list_read_usual.txt", "r");
    if (f)
    {
        head = task_list_read(f);
        ck_assert_ptr_nonnull(head);
        node_t *headd = head;
        ck_assert_int_eq(my_comparator(&data1, head->data), 0);
        head = head->next;
        ck_assert_int_eq(my_comparator(&data2, head->data), 0);
        head = head->next;
        ck_assert_int_eq(my_comparator(&data3, head->data), 0);
        head = head->next;
        ck_assert_ptr_null(head);
        task_list_free(headd);
        fclose(f);
    }
}
END_TEST

Suite* task_list_read_suite(void)
{
    Suite *s;
    TCase *tc_neg;
    TCase *tc_pos;

    s = suite_create("task_list_read");

    tc_neg = tcase_create("negatives");
    tcase_add_test(tc_neg, test_task_list_read_empty_file);
    tcase_add_test(tc_neg, test_task_list_read_empty_name_beg);
    tcase_add_test(tc_neg, test_task_list_read_empty_diff_mid);
    tcase_add_test(tc_neg, test_task_list_read_letter_diff_end);
    tcase_add_test(tc_neg, test_task_list_read_neg_diff_mid);
    tcase_add_test(tc_neg, test_task_list_read_float_diff_beg);
    suite_add_tcase(s, tc_neg);

    tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, test_task_list_read_1task);
    tcase_add_test(tc_pos, test_task_list_read_usual);
    suite_add_tcase(s, tc_pos);

    return s;
}

START_TEST(test_task_list_insert_sorted_beg_1t)
{
    data_t data1 = {"Test2", 2};
    node_t node1 = {&data1, NULL};

    node_t *head = &node1;

    data_t datael = {"Test2", 1};
    node_t nodeel = {&datael, NULL};

    task_list_insert_sorted(&head, &nodeel, my_comparator);

    ck_assert_int_eq(my_comparator(&datael, head->data), 0);
    head = head->next;
    ck_assert_int_eq(my_comparator(&data1, head->data), 0);
    head = head->next;
    ck_assert_ptr_null(head);
}
END_TEST

START_TEST(test_task_list_insert_sorted_mid_same)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};
    data_t data3 = {"Test3", 3};

    node_t node3 = {&data3, NULL};
    node_t node2 = {&data2, &node3};
    node_t node1 = {&data1, &node2};

    node_t *head = &node1;

    data_t datael = {"Test2", 2};
    node_t nodeel = {&datael, NULL};

    task_list_insert_sorted(&head, &nodeel, my_comparator);

    ck_assert_int_eq(my_comparator(&data1, head->data), 0);
    head = head->next;
    ck_assert_int_eq(my_comparator(&data2, head->data), 0);
    head = head->next;
    ck_assert_int_eq(my_comparator(&datael, head->data), 0);
    head = head->next;
    ck_assert_int_eq(my_comparator(&data3, head->data), 0);
    head = head->next;
    ck_assert_ptr_null(head);
}
END_TEST

START_TEST(test_task_list_insert_sorted_end_usual)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test2", 2};
    data_t data3 = {"Test3", 3};

    node_t node3 = {&data3, NULL};
    node_t node2 = {&data2, &node3};
    node_t node1 = {&data1, &node2};

    node_t *head = &node1;

    data_t datael = {"Test", 4};
    node_t nodeel = {&datael, NULL};

    task_list_insert_sorted(&head, &nodeel, my_comparator);

    ck_assert_int_eq(my_comparator(&data1, head->data), 0);
    head = head->next;
    ck_assert_int_eq(my_comparator(&data2, head->data), 0);
    head = head->next;
    ck_assert_int_eq(my_comparator(&data3, head->data), 0);
    head = head->next;
    ck_assert_int_eq(my_comparator(&datael, head->data), 0);
    head = head->next;
    ck_assert_ptr_null(head);
}
END_TEST

Suite* task_list_insert_sorted_suite(void)
{
    Suite *s;
    TCase *tc_neit;

    s = suite_create("task_list_insert_sorted");

    tc_neit = tcase_create("neitrals");
    tcase_add_test(tc_neit, test_task_list_insert_sorted_beg_1t);
    tcase_add_test(tc_neit, test_task_list_insert_sorted_mid_same);
    tcase_add_test(tc_neit, test_task_list_insert_sorted_end_usual);

    suite_add_tcase(s, tc_neit);

    return s;
}

START_TEST(test_create_deletion_list_usual)
{
    node_t *node3 = task_create_node("Test3", 3);
    if (node3)
    {
        node_t *node2 = task_create_node("Test2", 2);
        if (node2)
        {
            node_t *node1 = task_create_node("Test1", 1);
            if (node1)
            {

                node_t *head = node1;
                insert(&head, node2, NULL);
                insert(&head, node3, NULL);

                data_t **list = create_deletion_list(head);
                if (list)
                {
                    ck_assert_ptr_eq(list[0], node1->data);
                    ck_assert_ptr_eq(list[1], node2->data);
                    ck_assert_ptr_eq(list[2], node3->data);
                    ck_assert_ptr_null(list[3]);
                    free_deletion_list(list, head);
                    task_list_free(head);
                }
                else
                {
                   task_free(node3);
                   task_free(node2);
                   task_free(node1);
                }
            }
            else
            {
               task_free(node3);
               task_free(node2);
            }
        }
        else
            task_free(node3);
    }
}
END_TEST

START_TEST(test_create_deletion_list_one_elem)
{
    data_t data1 = {"Test1", 1};

    node_t node1 = {&data1, NULL};

    node_t *head = &node1;

    data_t **list = create_deletion_list(head);
    if (list)
    {
        ck_assert_ptr_eq(list[0], node1.data);
        ck_assert_ptr_null(list[1]);
        free(list);
    }
}
END_TEST

START_TEST(test_create_deletion_list_empty)
{

    node_t *head = NULL;

    data_t **list = create_deletion_list(head);
    if (list)
    {
        ck_assert_ptr_null(list[0]);
        free(list);
    }
}
END_TEST

Suite* create_deletion_list_suite(void)
{
    Suite *s;
    TCase *tc_neit;

    s = suite_create("create_deletion_list");

    tc_neit = tcase_create("neitrals");
    tcase_add_test(tc_neit, test_create_deletion_list_usual);
    tcase_add_test(tc_neit, test_create_deletion_list_one_elem);
    tcase_add_test(tc_neit, test_create_deletion_list_empty);

    suite_add_tcase(s, tc_neit);

    return s;
}
