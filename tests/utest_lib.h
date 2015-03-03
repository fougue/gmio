/* Adapted from minunit
 * See : http://www.jera.com/techinfo/jtns/jtn002.html
 *       http://c.learncodethehardway.org/book/ex30.html
 */

#ifndef UTEST_LIB_H
#define UTEST_LIB_H

#include <stdio.h>
#include <stdlib.h>

#define UTEST_SUITE_START() const char* message = NULL

#define UTEST_ASSERT(test) if (!(test))\
                           { printf("  ERROR : %s   (line = %i, file = %s)", #test, __LINE__, __FILE__);\
                             return #test; }

#define UTEST_ASSERT_MSG(test, message) if (!(test))\
                                        { printf(message); return message; }
                                        
#define UTEST_RUN(test) printf("\n-----%s", " " #test); \
                        message = test();\
                        tests_run++;\
                        if (message) return message;

#define UTEST_MAIN(name) \
        int main(int argc, char *argv[]) {\
            const char *result = NULL; \
            \
            printf("----\nRUNNING: %s\n", argv[0]);\
            result = name();\
            if (result != NULL) {\
                printf("\n\nFAILED: %s\n", result);\
            }\
            else {\
                printf("\n\nALL TESTS PASSED\n");\
            }\
            printf("Tests run: %d\n", tests_run);\
            exit(result != NULL);\
        }


static int tests_run;

#endif /* UTEST_LIB_H */
