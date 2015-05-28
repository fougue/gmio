/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougsys.fr
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

#include "../src/gmio_core/error.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_io.h"

#include <stddef.h>
#include <string.h>

static void add_triangle(
        void* cookie, uint32_t tri_id, const gmio_stl_triangle_t* triangle)
{
    GMIO_UNUSED(cookie);
    GMIO_UNUSED(tri_id);
    GMIO_UNUSED(triangle);
}

struct filepath_errorcode
{
    const char* filepath;
    int errorcode;
};
typedef struct filepath_errorcode filepath_errorcode_t;

const char* test_stl_read()
{
    const filepath_errorcode_t expected[] = {
        { "tests/models/file_empty", GMIO_STL_ERROR_UNKNOWN_FORMAT },
        { "tests/models/solid_4vertex.stla", GMIO_STL_ERROR_PARSING },
        { "tests/models/solid_anonymous_empty.stla", GMIO_ERROR_OK },
        { "tests/models/solid_empty.stla", GMIO_ERROR_OK },
        { "tests/models/solid_empty.stlb", GMIO_ERROR_OK },
        { "tests/models/solid_lack_z.stla", GMIO_STL_ERROR_PARSING },
        { "tests/models/solid_one_facet.stla", GMIO_ERROR_OK },
        { "tests/models/solid_one_facet.le_stlb", GMIO_ERROR_OK },
        { "tests/models/solid_one_facet_uppercase.stla", GMIO_ERROR_OK }
    };
    const size_t expected_count =
            sizeof(expected) / sizeof(filepath_errorcode_t);
    size_t i; /* for loop counter */
    gmio_stl_mesh_creator_t meshc = {0};

    meshc.add_triangle_func = &add_triangle;

    for (i = 0; i < expected_count; ++i) {
        const int err = gmio_stl_read_file(expected[i].filepath, &meshc, NULL);
        if (err != expected[i].errorcode) {
            printf("\nfilepath : %s\n"
                   "expected error : 0x%x\n"
                   "actual error   : 0x%x\n",
                   expected[i].filepath,
                   expected[i].errorcode,
                   err);
        }
        UTEST_ASSERT(err == expected[i].errorcode);
    }

    return NULL;
}


struct stl_triangle_array
{
    gmio_stl_triangle_t* ptr;
    uint32_t count;
};
typedef struct stl_triangle_array stl_triangle_array_t;

void stl_triangle_array_get_triangle(
        const void* cookie, uint32_t tri_id, gmio_stl_triangle_t* triangle)
{
    const stl_triangle_array_t* array = (const stl_triangle_array_t*)cookie;
    *triangle = array->ptr[tri_id];
}

void generate_stlb_tests_models()
{
    {
        FILE* outfile = fopen("tests/models/solid_empty.stlb", "wb");
        gmio_stream_t stream = gmio_stream_stdio(outfile);
        gmio_stlb_write_header(&stream, GMIO_ENDIANNESS_LITTLE, NULL, 0);
        fclose(outfile);
    }

    {
          gmio_stl_mesh_t mesh = {0};
          gmio_stl_triangle_t tri = {
              { 0.f, 0.f, 1.f },  /* normal */
              { 0.f, 0.f, 0.f },  /* v1 */
              { 10.f, 0.f, 0.f }, /* v2 */
              { 5.f, 10.f, 0.f }, /* v3 */
              0                   /* attr */
          };
          stl_triangle_array_t tri_array;

          tri_array.ptr = &tri;
          tri_array.count = 1;

          mesh.cookie = &tri_array;
          mesh.triangle_count = tri_array.count;
          mesh.get_triangle_func = &stl_triangle_array_get_triangle;

          gmio_stl_write_file(
                      GMIO_STL_FORMAT_BINARY_LE,
                      "tests/models/solid_one_facet.le_stlb",
                      &mesh,
                      NULL,
                      NULL);
    }
}

const char* all_tests()
{
    /*generate_stlb_tests_models();*/
    UTEST_SUITE_START();
    UTEST_RUN(test_stl_read);
    return NULL;
}

UTEST_MAIN(all_tests)
