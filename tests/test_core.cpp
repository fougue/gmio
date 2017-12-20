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

#include "../src/gmio_core/endian.h"
#include "../src/gmio_core/error.h"

namespace gmio {

static const char* TestCore_endian()
{
    UTEST_ASSERT(hostEndianness() == Endianness_Host);
GMIO_PRAGMA_MSVC_WARNING_PUSH_AND_DISABLE(4127)
    UTEST_ASSERT(Endianness_Little != Endianness_Big);
    UTEST_ASSERT(Endianness_Host == Endianness_Little
                 || Endianness_Host == Endianness_Big);
GMIO_PRAGMA_MSVC_WARNING_POP()
    return nullptr;
}

static const char* TestCore_error()
{
    UTEST_ASSERT(noError(Error_OK));
    UTEST_ASSERT(!error(Error_OK));

    UTEST_ASSERT(!noError(Error_Unknown));
    UTEST_ASSERT(error(Error_Unknown));

    return nullptr;
}

} // namespace gmio
