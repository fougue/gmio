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

#include "test_core.cpp"
#include "test_core_benchmark_fast_atof.cpp"
#include "test_core_internal.cpp"
#include "test_core_platform.cpp"

namespace gmio {

void TestCore_allTests()
{
    UTEST_RUN(TestCore_endian);
    UTEST_RUN(TestCore_error);

    UTEST_RUN(TestCore_globalHeader);
    UTEST_RUN(TestCore_compiler);

    UTEST_RUN(TestCoreInternal_byteSwap);
    UTEST_RUN(TestCoreInternal_byteCodec);
    UTEST_RUN(TestCoreInternal_fast_atof);
    UTEST_RUN(TestCoreInternal_locale_utils);
    UTEST_RUN(TestCoreInternal_itoa);
    UTEST_RUN(TestCoreInternal_ostringstream);
    UTEST_RUN(TestCoreInternal_safeCast);
    UTEST_RUN(TestCoreInternal_TextIStream);
    UTEST_RUN(TestCoreInternal_string_ascii_utils);
    UTEST_RUN(TestCoreInternal_benchmarkFastAtof);
    UTEST_RUN(TestCoreInternal_zip_utils);
    UTEST_RUN(TestCoreInternal_zlib_enumvalues);
    UTEST_RUN(TestCoreInternal_file_utils);
}

} // namespace gmio

UTEST_MAIN(gmio::TestCore_allTests)
