/* Adapted from minunit
 * See : http://www.jera.com/techinfo/jtns/jtn002.html
 *       http://c.learncodethehardway.org/book/ex30.html
 */

#pragma once

#include <cstdio>
#include <iostream>

#define UTEST_ASSERT(test) \
    if (!(test)) {\
        std::cerr << "\n  ERROR : " << #test\
                  << "   (line = " << std::dec << __LINE__ << ", file = " << __FILE__ << ")";\
        return #test;\
    }

#define UTEST_FAIL(message) \
    {\
        std::cerr << "\n  FAIL : " << #message\
                  << "   (line = " << std::dec << __LINE__ << ", file = " << __FILE__ << ")";\
        return "UTEST_FAIL()";\
    }

#define UTEST_ASSERT_MSG(test, message) \
    if (!(test)) {\
        std::cerr << message;\
        return message;\
    }

#define UTEST_EQUALS_OP__INTERNAL(lhs, rhs) ((lhs) == (rhs))
#define UTEST_EQUALS_STRCMP__INTERNAL(lhs, rhs) (std::strcmp((lhs), (rhs)) == 0)

#define UTEST_COMPARE__INTERNAL(expected, actual, func_cmp) \
    if (!func_cmp((expected), (actual))) {\
        std::cerr << "\n ERROR: comparison failed\n"\
                  << "        expected '" << (expected) << "'\n"\
                  << "        actual   '" << (actual) << "'\n"\
                  << "        line = " << std::dec << __LINE__ << ", file = " << __FILE__ << '\n';\
        return #expected " != " #actual;\
    }

#define UTEST_COMPARE(expected, actual) \
    UTEST_COMPARE__INTERNAL(expected, actual, UTEST_EQUALS_OP__INTERNAL)

#define UTEST_COMPARE_CSTR(expected, actual) \
    UTEST_COMPARE__INTERNAL(expected, actual, UTEST_EQUALS_STRCMP__INTERNAL)
