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

#include "stl_testcases.h"

#include "../src/gmio_core/error.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_infos.h"
#include "../src/gmio_core/internal/helper_stream.h"

#include <stdio.h>
#include <string.h>

static const char* __tstl__test_stl_infos(
        const struct stl_read_testcase* testcase)
{
    FILE* file = fopen(testcase->filepath, "rb");
    gmio_streamsize_t expected_size = testcase->expected_size;
    char stla_solid_name[512] = {0};
    struct gmio_stl_infos infos = {0};
    struct gmio_stream stream = gmio_stream_stdio(file);
    int error = GMIO_ERROR_OK;

    infos.stla_solidname = stla_solid_name;
    infos.stla_solidname_maxlen = sizeof(stla_solid_name) - 1;
    error = gmio_stl_infos_probe(&infos, &stream, GMIO_STL_INFO_FLAG_ALL, NULL);

    if (testcase->expected_size == -1)
        expected_size = gmio_stream_size(&stream);
    fclose(file);

    if (testcase->format != GMIO_STL_FORMAT_UNKNOWN) {
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
    }
    else {
        UTEST_COMPARE_INT(GMIO_STL_ERROR_UNKNOWN_FORMAT, error);
    }

    if (testcase->format == GMIO_STL_FORMAT_ASCII) {
        const size_t name_len = strlen(stla_solid_name);
        const gmio_streamsize_t name_len_ssize = name_len;
#if 0
        printf("expected_size=%d    "
               "name_len=%d    "
               "infos.size=%d    "
               "infos.solid_name=%s\n",
               expected_size, name_len, infos.size, infos.stla_solidname);
#endif
        UTEST_ASSERT((expected_size - name_len_ssize) <= infos.size);
        UTEST_ASSERT(infos.size <= (expected_size + name_len_ssize));
    }
    else {
        UTEST_COMPARE_INT(expected_size, infos.size);
    }

    return NULL;
}

static const char* test_stl_infos()
{
    const struct stl_read_testcase* testcase = stl_read_testcases_ptr();
    while (testcase != stl_read_testcases_ptr_end()) {
        const char* error = __tstl__test_stl_infos(testcase);
        if (error != NULL) {
            fprintf(stderr,
                    "\ntest_stl_infos()\n"
                    "     filepath : %s\n"
                    "     error : %s\n",
                    testcase->filepath, error);
            return error;
        }
        ++testcase;
    }
    return NULL;
}
