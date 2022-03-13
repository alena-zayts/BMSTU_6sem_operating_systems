#include "check_main.h"

START_TEST(test_my_getdelim_empty_first_n)
{
    char *str = NULL;
    size_t n = 0;
    int rc;
    FILE *f = fopen("unit_tests\\test_my_getdelim_empty_first_n.txt", "r");
    if (f)
    {
        rc = my_getdelim(&str, &n, '\n', f);
        ck_assert_int_eq(rc, ERRVALUE);
        ck_assert_int_eq((int) n, 0);
        ck_assert_ptr_null(str);
        fclose(f);
    }
}
END_TEST

START_TEST(test_my_getdelim_empty_mid_a)
{
    char *str = NULL;
    size_t n = 0;
    int rc;
    FILE *f = fopen("unit_tests\\test_my_getdelim_empty_mid_a.txt", "r");
    if (f)
    {
        if (my_getdelim(&str, &n, 'a', f) == OK)
        {
            free(str);
            str = NULL;
            n = 0;

            rc = my_getdelim(&str, &n, 'a', f);
            ck_assert_int_eq(rc, ERRVALUE);
            ck_assert_int_eq((int) n, 0);
            ck_assert_ptr_null(str);
            fclose(f);
        }
    }
}
END_TEST

START_TEST(test_my_getdelim_1symb)
{
    char *str = NULL;
    size_t n = 0;
    int rc;
    FILE *f = fopen("unit_tests\\test_my_getdelim_1symb.txt", "r");
    if (f)
    {
        rc = my_getdelim(&str, &n, '\n', f);
        ck_assert_int_eq(rc, OK);
        ck_assert_int_eq((int) n, 1);
        ck_assert_ptr_nonnull(str);
        ck_assert_int_eq(strcmp(str, "1"), 0);
        free(str);
        fclose(f);
    }
}
END_TEST

START_TEST(test_my_getdelim_10usual_dot)
{
    char *str = NULL;
    size_t n = 0;
    int rc;
    FILE *f = fopen("unit_tests\\test_my_getdelim_10usual_dot.txt", "r");
    if (f)
    {
        rc = my_getdelim(&str, &n, '.', f);
        ck_assert_int_eq(rc, OK);
        ck_assert_int_eq((int) n, 10);
        ck_assert_ptr_nonnull(str);
        ck_assert_int_eq(strcmp(str, "1234567890"), 0);
        free(str);
        fclose(f);
    }
}
END_TEST

Suite* my_getdelim_suite(void)
{
    Suite *s;
    TCase *tc_neg;
    TCase *tc_pos;

    s = suite_create("my_getdelim");

    tc_neg = tcase_create("negatives");
    tcase_add_test(tc_neg, test_my_getdelim_empty_first_n);
    tcase_add_test(tc_neg, test_my_getdelim_empty_mid_a);
    suite_add_tcase(s, tc_neg);

    tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, test_my_getdelim_1symb);
    tcase_add_test(tc_pos, test_my_getdelim_10usual_dot);
    suite_add_tcase(s, tc_pos);

    return s;
}
