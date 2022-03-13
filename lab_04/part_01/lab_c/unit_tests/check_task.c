#include "check_main.h"

//my_comarator
START_TEST(test_my_comparator_equal)
{
    data_t data1 = {"Test", 1};
    data_t data2 = {"Test", 1};

    int rc = my_comparator(&data1, &data2);

    ck_assert_int_eq(rc, 0);
}
END_TEST


START_TEST(test_my_comparator_diffeq_namesmaller)
{
    data_t data1 = {"Test", 1};
    data_t data2 = {"Test1", 1};

    int rc = my_comparator(&data1, &data2);

    ck_assert_int_eq(rc, -1);
}
END_TEST

START_TEST(test_my_comparator_diffeq_namebigger)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test", 1};

    int rc = my_comparator(&data1, &data2);

    ck_assert_int_eq(rc, 1);
}
END_TEST

START_TEST(test_my_comparator_diffbigger_nameeq)
{
    data_t data1 = {"Test", 2};
    data_t data2 = {"Test", 1};

    int rc = my_comparator(&data1, &data2);

    ck_assert_int_eq(rc, 1);
}
END_TEST

START_TEST(test_my_comparator_diffsmaller_nameeq)
{
    data_t data1 = {"Test", 1};
    data_t data2 = {"Test", 2};

    int rc = my_comparator(&data1, &data2);

    ck_assert_int_eq(rc, -1);
}
END_TEST

START_TEST(test_my_comparator_diffsmaller_namebigger)
{
    data_t data1 = {"Test1", 1};
    data_t data2 = {"Test", 2};

    int rc = my_comparator(&data1, &data2);

    ck_assert_int_eq(rc, -1);
}
END_TEST

START_TEST(test_my_comparator_diffbigger_namesmaller)
{
    data_t data1 = {"Test", 2};
    data_t data2 = {"Test1", 1};

    int rc = my_comparator(&data1, &data2);

    ck_assert_int_eq(rc, 1);
}
END_TEST

Suite* my_comparator_suite(void)
{
    Suite *s;
    TCase *tc_neit;

    s = suite_create("my_comparator");

    tc_neit = tcase_create("neitrals");
    tcase_add_test(tc_neit, test_my_comparator_equal);
    tcase_add_test(tc_neit, test_my_comparator_diffeq_namesmaller);
    tcase_add_test(tc_neit, test_my_comparator_diffeq_namebigger);
    tcase_add_test(tc_neit, test_my_comparator_diffbigger_nameeq);
    tcase_add_test(tc_neit, test_my_comparator_diffsmaller_nameeq);
    tcase_add_test(tc_neit, test_my_comparator_diffsmaller_namebigger);
    tcase_add_test(tc_neit, test_my_comparator_diffbigger_namesmaller);

    suite_add_tcase(s, tc_neit);

    return s;
}

START_TEST(test_task_create_node_usual)
{
    data_t data = {"Test", 1};
    node_t *node = task_create_node("Test", 1);
    if (node)
    {
        ck_assert_int_eq(my_comparator(&data, node->data), 0);
        ck_assert_ptr_null(node->next);
        task_free(node);
    }
}
END_TEST

START_TEST(test_task_create_node_1letter)
{
    data_t data = {"T", 1};
    node_t *node = task_create_node("T", 1);
    if (node)
    {
        ck_assert_int_eq(my_comparator(&data, node->data), 0);
        ck_assert_ptr_null(node->next);
        task_free(node);
    }
}
END_TEST

Suite* task_create_node_suite(void)
{
    Suite *s;
    TCase *tc_neit;

    s = suite_create("task_create_node");

    tc_neit = tcase_create("neitrals");
    tcase_add_test(tc_neit, test_task_create_node_usual);
    tcase_add_test(tc_neit, test_task_create_node_1letter);


    suite_add_tcase(s, tc_neit);

    return s;
}

START_TEST(test_task_read_empty_file)
{
    node_t *node = NULL;

    FILE *f = fopen("unit_tests\\test_task_read_empty_file.txt", "r");
    if (f)
    {
        node = task_read(f);
        ck_assert_ptr_null(node);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_read_empty_name)
{
    node_t *node = NULL;

    FILE *f = fopen("unit_tests\\test_task_read_empty_name.txt", "r");
    if (f)
    {
        node = task_read(f);
        ck_assert_ptr_null(node);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_read_empty_diff)
{
    node_t *node = NULL;

    FILE *f = fopen("unit_tests\\test_task_read_empty_diff.txt", "r");
    if (f)
    {
        node = task_read(f);
        ck_assert_ptr_null(node);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_read_float_diff)
{
    node_t *node = NULL;

    FILE *f = fopen("unit_tests\\test_task_read_float_diff.txt", "r");
    if (f)
    {
        node = task_read(f);
        ck_assert_ptr_null(node);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_read_letter_diff)
{
    node_t *node = NULL;

    FILE *f = fopen("unit_tests\\test_task_read_letter_diff.txt", "r");
    if (f)
    {
        node = task_read(f);
        ck_assert_ptr_null(node);
        fclose(f);
    }
}
END_TEST

START_TEST(test_task_read_1letter)
{
    data_t data_e = {"T", 1};
    node_t node_e = {&data_e, NULL};
    node_t *node = NULL;

    FILE *f = fopen("unit_tests\\test_task_read_1letter.txt", "r");
    if (f)
    {
        node = task_read(f);
        ck_assert_ptr_nonnull(node);
        ck_assert_int_eq(my_comparator(node_e.data, node->data), 0);
        fclose(f);
        task_free(node);
    }
}
END_TEST

START_TEST(test_task_read_usual)
{
    data_t data_e = {"TeST", 10};
    node_t node_e = {&data_e, NULL};
    node_t *node = NULL;

    FILE *f = fopen("unit_tests\\test_task_read_usual.txt", "r");
    if (f)
    {
        node = task_read(f);
        ck_assert_ptr_nonnull(node);
        ck_assert_int_eq(my_comparator(node_e.data, node->data), 0);
        fclose(f);
        task_free(node);
    }
}
END_TEST

Suite* task_read_suite(void)
{
    Suite *s;
    TCase *tc_neg;
    TCase *tc_pos;

    s = suite_create("task_read");

    tc_neg = tcase_create("negatives");
    tcase_add_test(tc_neg, test_task_read_empty_file);
    tcase_add_test(tc_neg, test_task_read_empty_name);
    tcase_add_test(tc_neg, test_task_read_empty_diff);
    tcase_add_test(tc_neg, test_task_read_float_diff);
    tcase_add_test(tc_neg, test_task_read_letter_diff);
    suite_add_tcase(s, tc_neg);

    tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, test_task_read_1letter);
    tcase_add_test(tc_pos, test_task_read_usual);
    suite_add_tcase(s, tc_pos);

    return s;
}

START_TEST(test_task_print_usual)
{
    data_t data1 = {"TeST", 10};
    node_t node1 = {&data1, NULL};
    node_t *node2 = NULL;

    FILE *f = fopen("unit_tests\\test_task_print_usual.txt", "w");
    if (f)
    {
        task_print(f, &node1);
        fclose(f);
        f = fopen("unit_tests\\test_task_print_usual.txt", "r");
        if (f)
        {
            node2 = task_read(f);
            ck_assert_ptr_nonnull(node2);
            ck_assert_int_eq(my_comparator(node2->data, node1.data), 0);
            fclose(f);
            task_free(node2);
        }
    }
}
END_TEST

Suite* task_print_suite(void)
{
    Suite *s;
    TCase *tc_neit;

    s = suite_create("task_print");

    tc_neit = tcase_create("neitrals");
    tcase_add_test(tc_neit, test_task_print_usual);

    suite_add_tcase(s, tc_neit);

    return s;
}
