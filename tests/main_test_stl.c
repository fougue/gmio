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

const char* test_stl_internal__rw_common();

const char* test_stl_infos();
const char* test_stl_read();
const char* test_stla_write();
const char* test_stlb_write_header();
const char* test_stlb_write();

const char* all_tests()
{
    UTEST_SUITE_START();

    UTEST_RUN(test_stl_internal__rw_common);

    UTEST_RUN(test_stl_infos);
    UTEST_RUN(test_stl_read);
    UTEST_RUN(test_stla_write);
    UTEST_RUN(test_stlb_write_header);
    UTEST_RUN(test_stlb_write);

    return NULL;
}
UTEST_MAIN(all_tests)
