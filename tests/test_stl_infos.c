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

#include "../src/gmio_core/error.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_infos.h"

#include <stdio.h>

struct gmio_test_stl_infos
{
    const char* filepath;
    enum gmio_stl_format format;
    uint32_t expected_facet_count;
    gmio_streamsize_t expected_size; /* -2: don't check size
                                      * -1: check against actual file size */
};

static const struct gmio_test_stl_infos tests[] = {
    { "models/file_empty", GMIO_STL_FORMAT_UNKNOWN, 0, 0 },
    { "models/solid_4vertex.stla", GMIO_STL_FORMAT_ASCII, 1, -2 },
    { "models/solid_anonymous_empty.stla", GMIO_STL_FORMAT_ASCII, 0, -2 },
    { "models/solid_empty.stla", GMIO_STL_FORMAT_ASCII, 0, -2 },
    { "models/solid_empty.stlb", GMIO_STL_FORMAT_BINARY_LE, 0, -1 },
    { "models/solid_empty_name_many_words.stla", GMIO_STL_FORMAT_ASCII, 0, -2 },
    { "models/solid_empty_name_many_words_single_letters.stla", GMIO_STL_FORMAT_ASCII, 0, -2 },
    { "models/solid_grabcad_arm11_link0_hb.le_stlb", GMIO_STL_FORMAT_BINARY_LE, 1436, -1 },
    { "models/solid_jburkardt_sphere.stla", GMIO_STL_FORMAT_ASCII, 228, -2 },
    { "models/solid_lack_z.stla", GMIO_STL_FORMAT_ASCII, 1, -2 },
    { "models/solid_one_facet.be_stlb", GMIO_STL_FORMAT_BINARY_BE, 1, -1 },
    { "models/solid_one_facet.le_stlb", GMIO_STL_FORMAT_BINARY_LE, 1, -1 },
    { "models/solid_one_facet.stla", GMIO_STL_FORMAT_ASCII, 1, -2 },
    { "models/solid_one_facet_uppercase.stla", GMIO_STL_FORMAT_ASCII, 1, -2 }
};

const char* generic_test_stl_infos(const struct gmio_test_stl_infos* test)
{
    FILE* file = fopen(test->filepath, "rb");
    gmio_streamsize_t expected_size = test->expected_size;
    struct gmio_stl_infos infos = {0};
    struct gmio_stream stream = gmio_stream_stdio(file);
    int error = GMIO_ERROR_OK;

    printf("\n%s\n", test->filepath);

    error = gmio_stl_infos_get(&infos, stream, GMIO_STL_INFO_FLAG_ALL, NULL);
    if (test->format != GMIO_STL_FORMAT_UNKNOWN) {
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
    }
    else {
        UTEST_COMPARE_INT(GMIO_STL_ERROR_UNKNOWN_FORMAT, error);
    }

    if (test->expected_size == -1)
        expected_size = gmio_stream_size(&stream);

    fclose(file);

    if (test->expected_size != -2)
        UTEST_COMPARE_UINT(expected_size, infos.size);

    return NULL;
}

const char* test_stl_infos()
{
    size_t i;
    for (i = 0; i < GMIO_ARRAY_SIZE(tests); ++i) {
        const char* error = generic_test_stl_infos(tests + i);
        if (error != NULL)
            return error;
    }

    return NULL;
}
