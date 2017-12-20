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

#include "../src/gmio_stl/stlb_header.h"

#include <cstring>

namespace gmio {

static const char* TestSTL_binaryHeaderFromString()
{
    const STL_BinaryHeader nullh = {};

    {
        const STL_BinaryHeader h = STL_binaryHeaderFromString("");
        UTEST_ASSERT(h == nullh);
    }

    {
        const char cstr[] = "gmio by Fougue";
        const STL_BinaryHeader h = STL_binaryHeaderFromString(cstr);
        UTEST_COMPARE_CSTR(reinterpret_cast<const char*>(h.data()), cstr);
        UTEST_ASSERT(std::memcmp(
                         &h.at(sizeof(cstr)),
                         &nullh.at(sizeof(cstr)),
                         STL_BinaryHeaderSize - sizeof(cstr))
                     == 0);
    }

    {
        const char cstr[] = { // size = 104(26*4)
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
        };
        const STL_BinaryHeader h = STL_binaryHeaderFromString(cstr);
        UTEST_ASSERT(std::strncmp(
                         reinterpret_cast<const char*>(h.data()),
                         cstr,
                         STL_BinaryHeaderSize)
                     == 0);
    }

    return nullptr;
}

static const char* TestSTL_binaryHeaderToString()
{
    {
        const STL_BinaryHeader nullh = {};
        const auto strh = STL_binaryHeaderToString(nullh, '\0');
        UTEST_ASSERT(std::memcmp(strh.data(), nullh.data(), STL_BinaryHeaderSize) == 0);
        UTEST_COMPARE(std::strlen(strh.data()), 0);
    }

    {
        const char cstr_ab[] = "abcdefghijklmnopqrstuvwxyz";
        const char cstr_ab_after_cpy[] = "abcde?ghijklmnopqrstuvwxyz";
        STL_BinaryHeader h = STL_binaryHeaderFromString(cstr_ab);
        h[5] = 7; // Bell
        const auto strh = STL_binaryHeaderToString(h, '?');
        UTEST_ASSERT(std::strncmp(cstr_ab_after_cpy,
                                  strh.data(),
                                  sizeof(cstr_ab_after_cpy) - 1)
                     == 0);
    }

    return nullptr;
}

} // namespace gmio
