/* Adapted from minunit
 * See : http://www.jera.com/techinfo/jtns/jtn002.html
 *       http://c.learncodethehardway.org/book/ex30.html
 */

#ifndef UTEST_ASSERT_H
#define UTEST_ASSERT_H

#include <stdio.h>

#define UTEST_ASSERT(test) \
    if (!(test)) {\
        printf("\n  ERROR : %s   (line = %i, file = %s)", #test, __LINE__, __FILE__);\
        return #test;\
    }

#define UTEST_FAIL(message) \
    {\
        printf("\n  FAIL: %s   (line = %i, file = %s)", #message, __LINE__, __FILE__);\
        return "UTEST_FAIL()";\
    }

#define UTEST_ASSERT_MSG(test, message) \
    if (!(test)) {\
        printf(message);\
        return message;\
    }

#define UTEST_EQUALS_OP__INTERNAL(lhs, rhs) ((lhs) == (rhs))
#define UTEST_EQUALS_STRCMP__INTERNAL(lhs, rhs) (strcmp((lhs), (rhs)) == 0)

#define UTEST_COMPARE__INTERNAL(expected, actual, func_cmp, fmt, title) \
    if (!func_cmp(expected, actual)) {\
        printf("\n  ERROR: %s comparison failed\n"\
               "               expected '" fmt "'\n"\
               "               actual   '" fmt "'\n"\
               "           line = %i, file = %s\n",\
               (title), (expected), (actual), __LINE__, __FILE__);\
        return #expected " != " #actual;\
    }

#define UTEST_COMPARE_INT(expected, actual) \
    UTEST_COMPARE__INTERNAL(expected, actual, UTEST_EQUALS_OP__INTERNAL, "%d", "int")

#define UTEST_COMPARE_UINT(expected, actual) \
    UTEST_COMPARE__INTERNAL(expected, actual, UTEST_EQUALS_OP__INTERNAL, "%u", "uint")

#define UTEST_COMPARE_CSTR(expected, actual) \
    UTEST_COMPARE__INTERNAL(expected, actual, UTEST_EQUALS_STRCMP__INTERNAL, "%s", "C-string")

#endif /* UTEST_ASSERT_H */
