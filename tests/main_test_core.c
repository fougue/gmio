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

#include "test_core.c"
#include "test_core_benchmark_fast_atof.c"
#include "test_core_internal.c"
#include "test_core_platform.c"

const char* all_tests()
{
    UTEST_SUITE_START();

    UTEST_RUN(test_core__buffer);
    UTEST_RUN(test_core__endian);
    UTEST_RUN(test_core__error);
    UTEST_RUN(test_core__stream);

    UTEST_RUN(test_platform__global_h);
    UTEST_RUN(test_platform__compiler);

    UTEST_RUN(test_internal__byte_swap);
    UTEST_RUN(test_internal__byte_codec);
    UTEST_RUN(test_internal__fast_atof);
    UTEST_RUN(test_internal__safe_cast);
    UTEST_RUN(test_internal__stringstream);
    UTEST_RUN(test_internal__string_utils);
    UTEST_RUN(test_internal__benchmark_gmio_fast_atof);

    return NULL;
}
UTEST_MAIN(all_tests)
