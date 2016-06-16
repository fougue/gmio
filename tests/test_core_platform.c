/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#include "utest_assert.h"

#include "../src/gmio_core/global.h"
#include "../src/gmio_core/stream.h"

#include <stddef.h>
#include <string.h>

/* Disable MSVC warning "conditional expression is constant" */
GMIO_PRAGMA_MSVC_WARNING_PUSH_AND_DISABLE(4127)

static const char* test_platform__global_h()
{
    UTEST_ASSERT(sizeof(int8_t) == 1);
    UTEST_ASSERT(sizeof(uint8_t) == 1);

    UTEST_ASSERT(sizeof(int16_t) == 2);
    UTEST_ASSERT(sizeof(uint16_t) == 2);

    UTEST_ASSERT(sizeof(int32_t) == 4);
    UTEST_ASSERT(sizeof(uint32_t) == 4);

#ifdef GMIO_HAVE_INT64_TYPE
    UTEST_ASSERT(sizeof(int64_t) == 8);
    UTEST_ASSERT(sizeof(uint64_t) == 8);
#endif

    UTEST_ASSERT(sizeof(float) == 4);
    UTEST_ASSERT(sizeof(double) == 8);

    return NULL;
}

static const char* test_platform__compiler()
{
    /* Check that universal zero initializer {0} works as expected
     *
     * See http://www.ex-parrot.com/~chris/random/initialise.html
     *
     * Depending on your version, GCC can incorrectly reports the warning
     * "missing braces around initializer [-Wmissing-braces]"
     * See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
     */
    {
        const struct gmio_stream null_bracket0 = {0};
        struct gmio_stream null_memset0;

        memset(&null_memset0, 0, sizeof(struct gmio_stream));

        UTEST_ASSERT(memcmp(
                         &null_bracket0,
                         &null_memset0,
                         sizeof(struct gmio_stream))
                     == 0);
    }

    /* Check sizeof() operator with fixed-size arrays */
    {
        uint8_t buff[4 * 1024]; /* 4KB */
        UTEST_ASSERT(sizeof(buff) == 4 * 1024);
    }

    /* gmio doesn't support platforms where NULL != 0 */
    UTEST_ASSERT(NULL == 0);

    return NULL;
}

GMIO_PRAGMA_MSVC_WARNING_POP()
