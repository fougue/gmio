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

#include <cstring>

// Disable MSVC warning "conditional expression is constant"
GMIO_PRAGMA_MSVC_WARNING_PUSH_AND_DISABLE(4127)

namespace gmio {

static const char* TestCore_globalHeader()
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

    return nullptr;
}

static const char* TestCore_compiler()
{
    // Check that universal zero initializer {} works as expected
    //
    // See http://www.ex-parrot.com/~chris/random/initialise.html
    //
    // Depending on your version, GCC can incorrectly reports the warning
    // "missing braces around initializer [-Wmissing-braces]"
    // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
    {
        struct Test {
          int a;
          int b;
          char tab[15];
          float c;
          double d;
        };
        const Test null_test_bracket = {};
        Test null_test_memset0;

        std::memset(&null_test_memset0, 0, sizeof(Test));

        UTEST_ASSERT(std::memcmp(
                         &null_test_bracket,
                         &null_test_memset0,
                         sizeof(Test))
                     == 0);
    }

    // Check sizeof() operator with fixed-size arrays
    {
        const uint8_t buff[4 * 1024] = {}; // 4KB
        UTEST_ASSERT(sizeof(buff) == 4 * 1024);
    }

    // gmio doesn't support platforms where NULL != 0
    UTEST_ASSERT(NULL == 0);

    return nullptr;
}

} // namespace gmio

GMIO_PRAGMA_MSVC_WARNING_POP()
