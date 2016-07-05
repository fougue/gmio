/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
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

#include <string.h>

static const char* test_stlb_header_str()
{
    const struct gmio_stlb_header null = {0};

    {
        const struct gmio_stlb_header h = gmio_stlb_header_str("");
        UTEST_ASSERT(memcmp(&h, &null, GMIO_STLB_HEADER_SIZE) == 0);
    }

    {
        const char cstr[] = "gmio by Fougue";
        const struct gmio_stlb_header h = gmio_stlb_header_str(cstr);
        UTEST_COMPARE_CSTR((const char*)h.data, cstr);
        UTEST_ASSERT(memcmp(
                         &h.data[sizeof(cstr)],
                         &null.data[sizeof(cstr)],
                         GMIO_STLB_HEADER_SIZE - sizeof(cstr))
                     == 0);
    }

    {
        const char cstr[] = { /* size = 104(26*4) */
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
        };
        const struct gmio_stlb_header h = gmio_stlb_header_str(cstr);
        UTEST_ASSERT(strncmp((const char*)h.data, cstr, GMIO_STLB_HEADER_SIZE)
                     == 0);
    }

    return NULL;
}

static const char* test_stlb_header_to_printable_str()
{
    {
        const struct gmio_stlb_header null = {0};
        char cstr[GMIO_STLB_HEADER_SIZE+1];
        gmio_stlb_header_to_printable_str(&null, cstr, 0);
        UTEST_ASSERT(memcmp(&cstr, &null, GMIO_STLB_HEADER_SIZE) == 0);
        UTEST_COMPARE_UINT(strlen(cstr), 0);
    }

    {
        char cstr[GMIO_STLB_HEADER_SIZE+1];
        const char cstr_ab[] = "abcdefghijklmnopqrstuvwxyz";
        const char cstr_ab_after_cpy[] = "abcde?ghijklmnopqrstuvwxyz";
        struct gmio_stlb_header h = gmio_stlb_header_str(cstr_ab);
        h.data[5] = 7; /* Bell */
        gmio_stlb_header_to_printable_str(&h, cstr, '?');
        UTEST_ASSERT(strncmp(cstr_ab_after_cpy,
                             cstr,
                             sizeof(cstr_ab_after_cpy) - 1)
                     == 0);
    }

    return NULL;
}
