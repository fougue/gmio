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

#include "core_utils.h"
#include "stl_utils.h"

#include "../src/gmio_core/error.h"
#include "../src/gmio_core/internal/min_max.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_io.h"
#include "../src/gmio_stl/stl_io_options.h"

#include <stddef.h>
#include <stdlib.h>

static const char stl_grabcad_arm11_filepath[] =
        "models/solid_grabcad_arm11_link0_hb.le_stlb";

struct stl_testcase_result
{
    char solid_name[2048];
};

void stl_testcase_result__begin_solid(
        void* cookie, const struct gmio_stl_mesh_creator_infos* infos)
{
    if (infos->format == GMIO_STL_FORMAT_ASCII) {
        struct stl_testcase_result* res = (struct stl_testcase_result*)cookie;
        if (res != NULL) {
            res->solid_name[0] = 0;
            if (infos->stla_solid_name != NULL)
                strcpy(res->solid_name, infos->stla_solid_name);
        }
    }
}

struct stl_testcase
{
    const char* filepath;
    int errorcode;
    enum gmio_stl_format format;
    const char* solid_name;
};

const char* test_stl_read()
{
    const struct stl_testcase expected[] = {
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
        { "models/solid_jburkardt_sphere.stla",
          GMIO_ERROR_OK,
          GMIO_STL_FORMAT_ASCII,
          "sphere"
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
            sizeof(expected) / sizeof(struct stl_testcase);
    size_t i; /* for loop counter */
    struct gmio_stl_mesh_creator mesh_creator = {0};
    struct stl_testcase_result result = {0};

    mesh_creator.cookie = &result;
    mesh_creator.func_begin_solid = &stl_testcase_result__begin_solid;
    mesh_creator.func_add_triangle = &gmio_stl_nop_add_triangle;

    for (i = 0; i < expected_count; ++i) {
        const enum gmio_stl_format format =
                gmio_stl_get_format_file(expected[i].filepath);
        const int err =
                gmio_stl_read_file(expected[i].filepath, mesh_creator, NULL);

        /* Check format */
        if (format != expected[i].format) {
            printf("\nfilepath : %s\n"
                   "expected format : %d\n"
                   "actual format   : %d\n",
                   expected[i].filepath,
                   expected[i].format,
                   format);
        }
        UTEST_COMPARE_UINT(expected[i].format, format);

        /* Check error code */
        if (err != expected[i].errorcode) {
            printf("\nfilepath : %s\n"
                   "expected error : 0x%x\n"
                   "actual error   : 0x%x\n",
                   expected[i].filepath,
                   expected[i].errorcode,
                   err);
        }
        UTEST_COMPARE_UINT(expected[i].errorcode, err);

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
            UTEST_COMPARE_CSTR(expected_name, result.solid_name);
        }
    }

    return NULL;
}

const char* test_stlb_write_header()
{
    const char* filepath = "temp/solid.stlb";
    struct gmio_stlb_header header = {0};
    const char* header_str = "temp/solid.stlb generated with gmio library";
    int error = GMIO_ERROR_OK;

    {
        FILE* outfile = fopen(filepath, "wb");
        struct gmio_stream stream = gmio_stream_stdio(outfile);
        memcpy(&header,
               header_str,
               GMIO_MIN(GMIO_STLB_HEADER_SIZE, strlen(header_str)));
        error = gmio_stlb_write_header(
                    &stream, GMIO_ENDIANNESS_LITTLE, &header, 0);
        fclose(outfile);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
    }

    {
        struct gmio_stl_data data = {0};
        error = gmio_stl_read_file(
                    filepath, gmio_stl_data_mesh_creator(&data), NULL);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
        UTEST_ASSERT(gmio_stlb_header_equal(&header, &data.header));
        UTEST_COMPARE_UINT(0, data.tri_array.count);
    }

    return NULL;
}

/* Safely closes the two files \p f1 and \p f2 */
static void fclose_2(FILE* f1, FILE* f2)
{
    if (f1 != NULL)
        fclose(f1);
    if (f2 != NULL)
        fclose(f2);
}

const char* test_stlb_write()
{
    const char* model_filepath = stl_grabcad_arm11_filepath;
    const char* model_filepath_out = "temp/solid.le_stlb";
    const char* model_filepath_out_be = "temp/solid.be_stlb";
    struct gmio_stl_data data = {0};
    int error = GMIO_ERROR_OK;

    /* Read input model file */
    {
        error = gmio_stl_read_file(
                    model_filepath, gmio_stl_data_mesh_creator(&data), NULL);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
    }

    /* Write back input model file
     * Write also the model file in big-endian STL format
     */
    {
        struct gmio_stl_write_options opts = {0};
        opts.stlb_header = data.header;
        error = gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_LE,
                    model_filepath_out,
                    gmio_stl_data_mesh(&data),
                    &opts);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);

        /* Big-endian version */
        error = gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_BE,
                    model_filepath_out_be,
                    gmio_stl_data_mesh(&data),
                    &opts);
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
        if (in == NULL || out == NULL) {
            fclose_2(in, out);
            perror("test_stlb_write()");
            UTEST_FAIL("fopen() error for in/out model files");
        }
        do {
            bytes_read_in = fread(buffer_in, 1, buff_size, in);
            bytes_read_out = fread(buffer_out, 1, buff_size, out);
            if (bytes_read_in != bytes_read_out) {
                fclose_2(in, out);
                UTEST_FAIL("Different byte count between in/out");
            }
            if (memcmp(buffer_in, buffer_out, buff_size) != 0) {
                fclose_2(in, out);
                UTEST_FAIL("Different buffer contents between in/out");
            }
        } while (!feof(in) && !feof(out)
                 && bytes_read_in > 0 && bytes_read_out > 0);
        fclose_2(in, out);
    }

    /* Check output LE/BE models are equal */
    {
        struct gmio_stl_data data_be = {0};
        error = gmio_stl_read_file(
                    model_filepath_out_be,
                    gmio_stl_data_mesh_creator(&data_be),
                    NULL);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
        UTEST_ASSERT(gmio_stlb_header_equal(&data.header, &data_be.header));
        UTEST_COMPARE_UINT(data.tri_array.count, data_be.tri_array.count);
        UTEST_ASSERT(memcmp(data.tri_array.ptr,
                            data_be.tri_array.ptr,
                            data.tri_array.count * sizeof(struct gmio_stl_triangle))
                         == 0);
        free(data_be.tri_array.ptr);
    }

    free(data.tri_array.ptr);

    return NULL;
}

const char* test_stla_write()
{
    const char* model_filepath = stl_grabcad_arm11_filepath;
    const char* model_filepath_out = "temp/solid.stla";
    struct gmio_stl_data data = {0};
    char header_str[GMIO_STLB_HEADER_SIZE + 1] = {0};
    int error = GMIO_ERROR_OK;

    /* Read input model file */
    {
        error = gmio_stl_read_file(
                    model_filepath, gmio_stl_data_mesh_creator(&data), NULL);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
    }

    /* Write the model to STL ascii format */
    {
        struct gmio_stl_write_options opts = {0};
        gmio_stlb_header_to_printable_str(&data.header, header_str, '_');
        opts.stla_solid_name = header_str;
        opts.stla_float32_prec = 7;
        opts.stla_float32_format = GMIO_FLOAT_TEXT_FORMAT_SHORTEST_LOWERCASE;
        error = gmio_stl_write_file(
                    GMIO_STL_FORMAT_ASCII,
                    model_filepath_out,
                    gmio_stl_data_mesh(&data),
                    &opts);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
    }

    /* Read the output STL ascii model */
    {
        char trim_header_str[sizeof(header_str)] = {0};
        struct gmio_stl_data data_stla = {0};
        size_t i = 0;
        strncpy(trim_header_str, header_str, sizeof(header_str));
        gmio_string_trim_from_end(trim_header_str, sizeof(header_str));
        error = gmio_stl_read_file(
                    model_filepath_out,
                    gmio_stl_data_mesh_creator(&data_stla),
                    NULL);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
        UTEST_COMPARE_UINT(data.tri_array.count, data_stla.tri_array.count);
        UTEST_COMPARE_CSTR(trim_header_str, data_stla.solid_name);
        for (i = 0; i < data.tri_array.count; ++i) {
            const struct gmio_stl_triangle* lhs = &data.tri_array.ptr[i];
            const struct gmio_stl_triangle* rhs = &data_stla.tri_array.ptr[i];
            const bool tri_equal = gmio_stl_triangle_equal(lhs, rhs, 5);
            UTEST_ASSERT(tri_equal);
        }
    }

    return NULL;
}

void generate_stlb_tests_models()
{
    {
        FILE* outfile = fopen("models/solid_empty.stlb", "wb");
        struct gmio_stream stream = gmio_stream_stdio(outfile);
        gmio_stlb_write_header(&stream, GMIO_ENDIANNESS_LITTLE, NULL, 0);
        fclose(outfile);
    }

    {
        struct gmio_stl_triangle tri = {
            { 0.f, 0.f, 1.f },  /* normal */
            { 0.f, 0.f, 0.f },  /* v1 */
            { 10.f, 0.f, 0.f }, /* v2 */
            { 5.f, 10.f, 0.f }, /* v3 */
            0                   /* attr */
        };
        struct gmio_stl_data data = {0};

        data.tri_array.ptr = &tri;
        data.tri_array.count = 1;

        gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_LE,
                    "models/solid_one_facet.le_stlb",
                    gmio_stl_data_mesh(&data),
                    NULL);
        gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_BE,
                    "models/solid_one_facet.be_stlb",
                    gmio_stl_data_mesh(&data),
                    NULL);
    }
}
