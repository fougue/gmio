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

#include "../src/gmio_core/error.h"
#include "../src/gmio_core/internal/min_max.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_io.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

GMIO_INLINE gmio_bool_t gmio_stlb_header_equal(
        const gmio_stlb_header_t* lhs, const gmio_stlb_header_t* rhs)
{
    return memcmp(lhs, rhs, GMIO_STLB_HEADER_SIZE) == 0;
}

struct stl_testcase_result
{
    char solid_name[2048];
};
typedef struct stl_testcase_result stl_testcase_result_t;

void ascii_begin_solid(
        void* cookie, size_t stream_size, const char* solid_name)
{
    stl_testcase_result_t* res = (stl_testcase_result_t*)cookie;
    GMIO_UNUSED(stream_size);
    if (res != NULL) {
        res->solid_name[0] = 0;
        if (solid_name != NULL)
            strcpy(&res->solid_name[0], solid_name);
    }
}

static void add_triangle(
        void* cookie, uint32_t tri_id, const gmio_stl_triangle_t* triangle)
{
    GMIO_UNUSED(cookie);
    GMIO_UNUSED(tri_id);
    GMIO_UNUSED(triangle);
}

struct stl_testcase
{
    const char* filepath;
    int errorcode;
    gmio_stl_format_t format;
    const char* solid_name;
};
typedef struct stl_testcase stl_testcase;

const char* test_stl_read()
{
    const stl_testcase expected[] = {
        { "models/file_empty",
          GMIO_STL_ERROR_UNKNOWN_FORMAT,
          GMIO_STL_FORMAT_UNKNOWN,
          NULL
        },
        { "models/solid_4vertex.stla",
          GMIO_STL_ERROR_PARSING,
          GMIO_STL_FORMAT_ASCII,
          NULL
        },
        { "models/solid_anonymous_empty.stla",
          GMIO_ERROR_OK,
          GMIO_STL_FORMAT_ASCII,
          NULL
        },
        { "models/solid_empty.stla",
          GMIO_ERROR_OK,
          GMIO_STL_FORMAT_ASCII,
          "emptysolid"
        },
        { "models/solid_empty.stlb",
          GMIO_ERROR_OK,
          GMIO_STL_FORMAT_BINARY_LE,
          NULL
        },
        { "models/solid_empty_name_many_words.stla",
          GMIO_ERROR_OK,
          GMIO_STL_FORMAT_ASCII,
          "name with multiple words"
        },
        { "models/solid_empty_name_many_words_single_letters.stla",
          GMIO_ERROR_OK,
          GMIO_STL_FORMAT_ASCII,
          "a b c d e f\t\tg h"
        },
        { "models/solid_grabcad_arm11_link0_hb.le_stlb",
          GMIO_ERROR_OK,
          GMIO_STL_FORMAT_BINARY_LE,
          NULL
        },
        { "models/solid_lack_z.stla",
          GMIO_STL_ERROR_PARSING,
          GMIO_STL_FORMAT_ASCII,
          NULL
        },
        { "models/solid_one_facet.stla",
          GMIO_ERROR_OK,
          GMIO_STL_FORMAT_ASCII,
          NULL
        },
        { "models/solid_one_facet.le_stlb",
          GMIO_ERROR_OK,
          GMIO_STL_FORMAT_BINARY_LE,
          NULL
        },
        { "models/solid_one_facet.be_stlb",
          GMIO_ERROR_OK,
          GMIO_STL_FORMAT_BINARY_BE,
          NULL
        },
        { "models/solid_one_facet_uppercase.stla",
          GMIO_ERROR_OK,
          GMIO_STL_FORMAT_ASCII,
          NULL
        }
    };
    const size_t expected_count =
            sizeof(expected) / sizeof(stl_testcase);
    size_t i; /* for loop counter */
    gmio_stl_mesh_creator_t meshc = {0};
    stl_testcase_result_t result = {0};

    meshc.cookie = &result;
    meshc.func_ascii_begin_solid = &ascii_begin_solid;
    meshc.func_add_triangle = &add_triangle;

    for (i = 0; i < expected_count; ++i) {
        const gmio_stl_format_t format =
                gmio_stl_get_format_file(expected[i].filepath);
        const int err =
                gmio_stl_read_file(expected[i].filepath, &meshc, NULL);

        /* Check format */
        if (format != expected[i].format) {
            printf("\nfilepath : %s\n"
                   "expected format : %d\n"
                   "actual format   : %d\n",
                   expected[i].filepath,
                   expected[i].format,
                   format);
        }
        UTEST_ASSERT(format == expected[i].format);

        /* Check error code */
        if (err != expected[i].errorcode) {
            printf("\nfilepath : %s\n"
                   "expected error : 0x%x\n"
                   "actual error   : 0x%x\n",
                   expected[i].filepath,
                   expected[i].errorcode,
                   err);
        }
        UTEST_ASSERT(err == expected[i].errorcode);

        /* Check solid name */
        if (expected[i].format == GMIO_STL_FORMAT_ASCII) {
            const char* expected_name =
                    expected[i].solid_name != NULL ? expected[i].solid_name : "";
            if (strcmp(result.solid_name, expected_name) != 0) {
                printf("\nfilepath : %s\n"
                       "expected solidname : %s\n"
                       "actual solidname   : %s\n",
                       expected[i].filepath,
                       expected_name,
                       result.solid_name);
            }
            UTEST_ASSERT(strcmp(result.solid_name, expected_name) == 0);
        }
    }

    return NULL;
}

struct stl_triangle_array
{
    gmio_stl_triangle_t* ptr;
    uint32_t count;
};
typedef struct stl_triangle_array stl_triangle_array_t;

struct stl_io_cookie
{
    gmio_stlb_header_t header;
    stl_triangle_array_t tri_array;
};
typedef struct stl_io_cookie stl_io_cookie_t;

static void stl_io_cookie__binary_begin_solid(
        void* cookie, uint32_t tri_count, const gmio_stlb_header_t* header)
{
    stl_io_cookie_t* io_cookie = (stl_io_cookie_t*)cookie;
    memcpy(&io_cookie->header, header, GMIO_STLB_HEADER_SIZE);
    io_cookie->tri_array.count = tri_count;
    if (tri_count > 0) {
        io_cookie->tri_array.ptr =
                (gmio_stl_triangle_t*)malloc(tri_count * sizeof(gmio_stl_triangle_t));

    }
}

static void stl_io_cookie__add_triangle(
        void* cookie, uint32_t tri_id, const gmio_stl_triangle_t* triangle)
{
    stl_io_cookie_t* io_cookie = (stl_io_cookie_t*)cookie;
    io_cookie->tri_array.ptr[tri_id] = *triangle;
}

static void stl_io_cookie__get_triangle(
        const void* cookie, uint32_t tri_id, gmio_stl_triangle_t* triangle)
{
    const stl_io_cookie_t* io_cookie = (const stl_io_cookie_t*)cookie;
    *triangle = io_cookie->tri_array.ptr[tri_id];
}

static gmio_stl_mesh_creator_t stl_io_mesh_creator(stl_io_cookie_t* cookie)
{
    gmio_stl_mesh_creator_t creator = {0};
    creator.cookie = cookie;
    creator.func_binary_begin_solid = &stl_io_cookie__binary_begin_solid;
    creator.func_add_triangle = &stl_io_cookie__add_triangle;
    return creator;
}

static gmio_stl_mesh_t stl_io_mesh(const stl_io_cookie_t* cookie)
{
    gmio_stl_mesh_t mesh = {0};
    mesh.cookie = cookie;
    mesh.triangle_count = cookie->tri_array.count;
    mesh.func_get_triangle = &stl_io_cookie__get_triangle;
    return mesh;
}

const char* test_stlb_write_header()
{
    const char* filepath = "temp/solid.stlb";
    gmio_stlb_header_t header = { 0 };
    const char* header_str = "temp/solid.stlb generated with gmio library";
    int error = GMIO_ERROR_OK;

    {
        FILE* outfile = fopen(filepath, "wb");
        gmio_stream_t stream = gmio_stream_stdio(outfile);
        memcpy(&header,
               header_str,
               GMIO_MIN(GMIO_STLB_HEADER_SIZE, strlen(header_str)));
        error = gmio_stlb_write_header(
                    &stream, GMIO_ENDIANNESS_LITTLE, &header, 0);
        fclose(outfile);
        UTEST_ASSERT(error == GMIO_ERROR_OK);
    }

    {
        stl_io_cookie_t cookie = {0};
        gmio_stl_mesh_creator_t mesh_creator = stl_io_mesh_creator(&cookie);
        error = gmio_stl_read_file(filepath, &mesh_creator, NULL);
        UTEST_ASSERT(error == GMIO_ERROR_OK);
        UTEST_ASSERT(gmio_stlb_header_equal(&header, &cookie.header));
        UTEST_ASSERT(cookie.tri_array.count == 0);
    }

    return NULL;
}

const char* test_stlb_write()
{
    const char* model_filepath = "models/solid_grabcad_arm11_link0_hb.le_stlb";
    const char* model_filepath_out = "temp/solid_grabcad_arm11_link0_hb.le_stlb";
    const char* model_filepath_out_be = "temp/solid_grabcad_arm11_link0_hb.be_stlb";
    stl_io_cookie_t cookie = {0};
    int error = GMIO_ERROR_OK;

    /* Read input model file */
    {
        gmio_stl_mesh_creator_t mesh_creator = stl_io_mesh_creator(&cookie);
        error = gmio_stl_read_file(model_filepath, &mesh_creator, NULL);
        UTEST_ASSERT(error == GMIO_ERROR_OK);
    }

    /* Write back input model file
     * Write also the model file in big-endian STL format
     */
    {
        gmio_stl_write_options_t options = {0};
        const gmio_stl_mesh_t mesh = stl_io_mesh(&cookie);
        options.stlb_header_data = &cookie.header;
        error = gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_LE, model_filepath_out, &mesh, NULL, &options);
        UTEST_ASSERT(error == GMIO_ERROR_OK);

        /* Big-endian version */
        error = gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_BE, model_filepath_out_be, &mesh, NULL, &options);
    }

    /* Check input and output models are equal */
    {
        uint8_t buffer_in[2048] = {0};
        uint8_t buffer_out[2048] = {0};
        const size_t buff_size = 2048;
        size_t bytes_read_in = 0;
        size_t bytes_read_out = 0;
        FILE* in = fopen(model_filepath, "rb");
        FILE* out = fopen(model_filepath_out, "rb");
        UTEST_ASSERT(in != NULL && out != NULL);
        do {
            bytes_read_in = fread(&buffer_in[0], 1, buff_size, in);
            bytes_read_out = fread(&buffer_out[0], 1, buff_size, out);
            UTEST_ASSERT(bytes_read_in == bytes_read_out);
            UTEST_ASSERT(memcmp(&buffer_in[0], &buffer_out[0], buff_size) == 0);
        } while (!feof(in) && !feof(out)
                 && bytes_read_in > 0 && bytes_read_out > 0);
    }

    /* Check output LE/BE models are equal */
    {
        const uint32_t le_tri_count = cookie.tri_array.count;
        const gmio_stl_triangle_t* le_tri_ptr = cookie.tri_array.ptr;
        stl_io_cookie_t cookie_be = {0};
        gmio_stl_mesh_creator_t mesh_creator = stl_io_mesh_creator(&cookie_be);
        error = gmio_stl_read_file(model_filepath_out_be, &mesh_creator, NULL);
        UTEST_ASSERT(error == GMIO_ERROR_OK);
        UTEST_ASSERT(gmio_stlb_header_equal(&cookie.header, &cookie_be.header));
        UTEST_ASSERT(le_tri_count == cookie_be.tri_array.count);
        UTEST_ASSERT(memcmp(le_tri_ptr, cookie_be.tri_array.ptr, le_tri_count) == 0);
        free(cookie_be.tri_array.ptr);
    }

    free(cookie.tri_array.ptr);

    return NULL;
}

void generate_stlb_tests_models()
{
    {
        FILE* outfile = fopen("models/solid_empty.stlb", "wb");
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
          stl_io_cookie_t io_cookie = {0};

          io_cookie.tri_array.ptr = &tri;
          io_cookie.tri_array.count = 1;

          mesh.cookie = &io_cookie;
          mesh.triangle_count = io_cookie.tri_array.count;
          mesh.func_get_triangle = &stl_io_cookie__get_triangle;

          gmio_stl_write_file(
                      GMIO_STL_FORMAT_BINARY_LE,
                      "models/solid_one_facet.le_stlb",
                      &mesh,
                      NULL,
                      NULL);
          gmio_stl_write_file(
                      GMIO_STL_FORMAT_BINARY_BE,
                      "models/solid_one_facet.be_stlb",
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
    UTEST_RUN(test_stlb_write_header);
    UTEST_RUN(test_stlb_write);
    return NULL;
}

UTEST_MAIN(all_tests)
