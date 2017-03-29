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

#include "utest_lib.h"

#include "test_core.c"
#include "test_core_benchmark_fast_atof.c"
#include "test_core_internal.c"
#include "test_core_platform.c"

void all_tests()
{
    UTEST_RUN(test_core__buffer);
    UTEST_RUN(test_core__endian);
    UTEST_RUN(test_core__error);
    UTEST_RUN(test_core__stream);

    UTEST_RUN(test_platform__global_h);
    UTEST_RUN(test_platform__compiler);

    UTEST_RUN(test_internal__byte_swap);
    UTEST_RUN(test_internal__byte_codec);
    UTEST_RUN(test_internal__const_string);
    UTEST_RUN(test_internal__fast_atof);
    UTEST_RUN(test_internal__locale_utils);
    UTEST_RUN(test_internal__error_check);
    UTEST_RUN(test_internal__itoa);
    UTEST_RUN(test_internal__ostringstream);
    UTEST_RUN(test_internal__safe_cast);
    UTEST_RUN(test_internal__stringstream);
    UTEST_RUN(test_internal__string_ascii_utils);
    UTEST_RUN(test_internal__benchmark_gmio_fast_atof);
    UTEST_RUN(test_internal__zip_utils);
    UTEST_RUN(test_internal__zlib_enumvalues);
    UTEST_RUN(test_internal__file_utils);
}
UTEST_MAIN(all_tests)
