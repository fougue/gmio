/* Adapted from minunit
 * See : http://www.jera.com/techinfo/jtns/jtn002.html
 *       http://c.learncodethehardway.org/book/ex30.html
 */

#pragma once

#include "utest_assert.h"
#include <stdlib.h>

#define UTEST_RUN(func_run_utest) \
        {\
            printf("\n-----%s", " " #func_run_utest); \
            ++test_count;\
            const char* str_error = func_run_utest();\
            if (str_error == NULL)\
                ++test_ok_count;\
            else\
                printf("  FAILED: %s\n", str_error);\
        }

#define UTEST_MAIN(func_run_utests) \
        int main(int argc, char *argv[]) {\
            const char* prg_test_name = argv[0];\
            (void)argc;\
            test_count = test_ok_count = 0;\
            printf("----\nRUNNING: %s\n", prg_test_name);\
            func_run_utests();\
            printf("\n\nRESULT: %s\n"\
                   "        tested: %d  |  passed: %d  |  failed: %d\n",\
                   prg_test_name,\
                   test_count, test_ok_count, test_count - test_ok_count);\
            if (test_count > 0) {\
                if (test_ok_count == test_count)\
                    printf("        ALL TESTS PASSED\n");\
                else\
                    printf("        TEST FAILURE\n");\
            }\
            exit(test_ok_count != test_count);\
        }


static int test_count;
static int test_ok_count;
