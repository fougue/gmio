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

#include "stl_testcases.h"

#include "../src/gmio_core/error.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_infos.h"

#include <cstdio>
#include <iostream>

namespace gmio {

namespace {

const char* STL_checkReadTestCase(const STL_ReadTestCase& testcase)
{
    FILE* file = std::fopen(testcase.filepath, "rb");
    const Result<STL_Infos> infos =
            STL_probeInfos(FILE_funcReadData(file), STL_InfoFlag_All);
    const uint64_t expected_size =
            testcase.expected_size != -1 ?
                testcase.expected_size :
                FILE_size(file);
    std::fclose(file);

    if (testcase.format != STL_Format_Unknown) {
        UTEST_COMPARE(Error_OK, infos.errorCode());
        UTEST_COMPARE(testcase.expected_facet_count, infos.data().facet_count);
    }
    else {
        UTEST_COMPARE(STL_Error_UnknownFormat, infos.errorCode());
    }

    if (testcase.format == STL_Format_Ascii) {
        const size_t name_len = infos.data().ascii_solidname.length();
#if 0
        std::cout << "expected_size=" << expected_size << "    "
                  << "infos.size=" << infos.data().size << "    "
                  << "infos.ascii_solidname" << infos.data().ascii_solidname << "    "
                  << "infos.ascii_solidname.length=" << name_len << "\n";
#endif
        UTEST_ASSERT((expected_size - name_len) <= infos.data().size);
        UTEST_ASSERT(infos.data().size <= (expected_size + name_len));
    }
    else if (testcase.format & STL_Format_TagBinary) {
        UTEST_COMPARE(expected_size, infos.data().size);
    }

    return nullptr;
}

} // namespace

static const char* TestSTL_probeInfos()
{
    for (const STL_ReadTestCase& testcase : STL_spanReadTestCase()) {
        const char* error = STL_checkReadTestCase(testcase);
        if (error != nullptr) {
            std::cerr << "\nTestSTL_probeInfos()\n"
                      << "     filepath : " << testcase.filepath << "\n"
                      << "     error : " << error << "\n";
            return error;
        }
    }
    return nullptr;
}

} // namespace gmio
