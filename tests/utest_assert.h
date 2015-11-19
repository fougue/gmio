/* Adapted from minunit
 * See : http://www.jera.com/techinfo/jtns/jtn002.html
 *       http://c.learncodethehardway.org/book/ex30.html
 */

#ifndef UTEST_ASSERT_H
#define UTEST_ASSERT_H

#include <stdio.h>

#define UTEST_ASSERT(test) \
    if (!(test)) {\
        printf("  ERROR : %s   (line = %i, file = %s)", #test, __LINE__, __FILE__);\
        return #test;\
    }

#define UTEST_FAIL(message) \
    {\
        printf("  FAIL : %s   (line = %i, file = %s)", #message, __LINE__, __FILE__);\
        return "UTEST_FAIL()";\
    }

#define UTEST_ASSERT_MSG(test, message) \
    if (!(test)) {\
        printf(message);\
        return message;\
    }

#endif /* UTEST_ASSERT_H */
