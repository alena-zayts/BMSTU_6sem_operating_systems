#ifndef CHECK_MAIN_H
#define CHECK_MAIN_H

#include <check.h>
#include <assert.h>
#include <stdlib.h>
#include "pop_front.h"
#include "insert.h"
#include "remove_duplicates.h"
#include "sort.h"
#include "task.h"
#include "task_list.h"

int nodes_eq(node_t *x, node_t *y);

Suite *pop_front_suite(void);
Suite *insert_suite(void);
Suite *remove_duplicates_suite(void);

Suite *front_back_split_suite(void);
Suite* sorted_merge_suite(void);
Suite* sort_suite(void);


Suite* my_comparator_suite(void);
Suite* my_getdelim_suite(void);
Suite* task_create_node_suite(void);
Suite* task_read_suite(void);
Suite* task_print_suite(void);

Suite* task_list_read_suite(void);
Suite* task_list_insert_sorted_suite(void);
Suite* create_deletion_list_suite(void);

#endif // CHECK_MAIN_H
