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

const char* test_core__buffer();
const char* test_core__endian();
const char* test_core__error();
const char* test_core__stream();

const char* test_internal__byte_swap();
const char* test_internal__byte_codec();
const char* test_internal__fast_atof();
const char* test_internal__safe_cast();
const char* test_internal__string_parse();
const char* test_internal__string_utils();

const char* test_platform__alignment();
const char* test_platform__global_h();
const char* test_platform__compiler();

const char* all_tests()
{
    UTEST_SUITE_START();

    UTEST_RUN(test_core__buffer);
    UTEST_RUN(test_core__endian);
    UTEST_RUN(test_core__error);
    UTEST_RUN(test_core__stream);

    UTEST_RUN(test_internal__byte_swap);
    UTEST_RUN(test_internal__byte_codec);
    UTEST_RUN(test_internal__fast_atof);
    UTEST_RUN(test_internal__safe_cast);
    UTEST_RUN(test_internal__string_parse);
    UTEST_RUN(test_internal__string_utils);

    UTEST_RUN(test_platform__alignment);
    UTEST_RUN(test_platform__global_h);
    UTEST_RUN(test_platform__compiler);

    return NULL;
}
UTEST_MAIN(all_tests)
