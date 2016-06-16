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
