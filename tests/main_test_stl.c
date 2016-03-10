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

#include "utest_lib.h"

#include "../src/gmio_core/memblock.h"

const char* test_stl_coords_packing();
const char* test_stl_triangle_packing();
const char* test_stl_triangle_compute_normal();

const char* test_stl_internal__rw_common();

const char* test_stl_infos();

const char* test_stl_read();
const char* test_stl_read_multi_solid();
const char* test_stla_write();
const char* test_stlb_read();
const char* test_stlb_write();
const char* test_stlb_write_header();

const char* test_stlb_header_str();
const char* test_stlb_header_to_printable_str();

void generate_stlb_tests_models();

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

    UTEST_RUN(test_stl_coords_packing);
    UTEST_RUN(test_stl_triangle_packing);
    UTEST_RUN(test_stl_triangle_compute_normal);

    UTEST_RUN(test_stl_internal__rw_common);

    UTEST_RUN(test_stl_infos);

    UTEST_RUN(test_stl_read);
    UTEST_RUN(test_stl_read_multi_solid);
    UTEST_RUN(test_stla_write);
    UTEST_RUN(test_stlb_read);
    UTEST_RUN(test_stlb_write);
    UTEST_RUN(test_stlb_write_header);

    UTEST_RUN(test_stlb_header_str);
    UTEST_RUN(test_stlb_header_to_printable_str);

#if 0
    generate_stlb_tests_models();
#endif

    return NULL;
}
UTEST_MAIN(all_tests)
