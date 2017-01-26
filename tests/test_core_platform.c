/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
