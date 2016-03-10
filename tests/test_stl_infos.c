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

#include "stl_testcases.h"

#include "../src/gmio_core/error.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_infos.h"

#include <stdio.h>
#include <string.h>

const char* generic_test_stl_infos(const struct stl_read_testcase* testcase)
{
    FILE* file = fopen(testcase->filepath, "rb");
    gmio_streamsize_t expected_size = testcase->expected_size;
    char stla_solid_name[512] = {0};
    struct gmio_stl_infos infos = {0};
    struct gmio_stream stream = gmio_stream_stdio(file);
    int error = GMIO_ERROR_OK;

    infos.stla_solidname = stla_solid_name;
    infos.stla_solidname_maxlen = sizeof(stla_solid_name) - 1;
    error = gmio_stl_infos_get(&infos, &stream, GMIO_STL_INFO_FLAG_ALL, NULL);

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

const char* test_stl_infos()
{
    const struct stl_read_testcase* testcase = stl_read_testcases_ptr();
    while (testcase != stl_read_testcases_ptr_end()) {
        const char* error = generic_test_stl_infos(testcase);
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
