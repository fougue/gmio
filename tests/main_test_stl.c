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

#include "utest_lib.h"

#include "../src/gmio_core/memblock.h"

#include "test_stl_infos.c"
#include "test_stl_internal.c"
#include "test_stl_io.c"
#include "test_stl_triangle.c"
#include "test_stlb_header.c"

/* Static memblock */
struct gmio_memblock gmio_memblock_for_tests()
{
    static uint8_t buff[1024]; /* 1KB */
    return gmio_memblock(buff, sizeof(buff), NULL);
}

const char* all_tests()
{
    UTEST_SUITE_START();

    gmio_memblock_set_default_constructor(gmio_memblock_for_tests);

#if 0
    generate_stlb_tests_models();
#endif

    UTEST_RUN(test_stl_coords_packing);
    UTEST_RUN(test_stl_triangle_packing);
    UTEST_RUN(test_stl_triangle_compute_normal);

    UTEST_RUN(test_stl_internal__error_check);

    UTEST_RUN(test_stl_infos);

    UTEST_RUN(test_stl_read);
    UTEST_RUN(test_stl_read_multi_solid);
    UTEST_RUN(test_stla_lc_numeric);
    UTEST_RUN(test_stla_write);
    UTEST_RUN(test_stlb_read);
    UTEST_RUN(test_stlb_write);
    UTEST_RUN(test_stlb_header_write);

    UTEST_RUN(test_stlb_header_str);
    UTEST_RUN(test_stlb_header_to_printable_str);

    return NULL;
}
UTEST_MAIN(all_tests)
