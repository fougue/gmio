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

#include "core_utils.h"
#include "stl_testcases.h"
#include "stl_utils.h"

#include "../src/gmio_core/error.h"
#include "../src/gmio_core/internal/helper_stream.h"
#include "../src/gmio_core/internal/locale_utils.h"
#include "../src/gmio_core/internal/min_max.h"
#include "../src/gmio_core/internal/string.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_infos.h"
#include "../src/gmio_stl/stl_io.h"
#include "../src/gmio_stl/stl_io_options.h"

#include <locale.h>
#include <stddef.h>
#include <stdlib.h>

struct __tstl__testcase_result
{
    char solid_name[2048];
};

static void __tstl__testcase_result__begin_solid(
        void* cookie, const struct gmio_stl_mesh_creator_infos* infos)
{
    if (infos->format == GMIO_STL_FORMAT_ASCII) {
        struct __tstl__testcase_result* res =
                (struct __tstl__testcase_result*)cookie;
        if (res != NULL) {
            res->solid_name[0] = 0;
            if (infos->stla_solid_name != NULL)
                gmio_cstr_copy(
                            res->solid_name,
                            sizeof(res->solid_name),
                            infos->stla_solid_name,
                            strlen(infos->stla_solid_name));
        }
    }
}

static const char* test_stl_read()
{
    const struct stl_read_testcase* testcase = stl_read_testcases_ptr();
    struct gmio_stl_mesh_creator mesh_creator = {0};
    struct __tstl__testcase_result result = {0};

    mesh_creator.cookie = &result;
    mesh_creator.func_begin_solid = &__tstl__testcase_result__begin_solid;
    mesh_creator.func_add_triangle = &gmio_stl_nop_add_triangle;

    while (testcase != stl_read_testcases_ptr_end()) {
        const enum gmio_stl_format format =
                gmio_stl_format_probe_file(testcase->filepath);
        const int err =
                gmio_stl_read_file(testcase->filepath, &mesh_creator, NULL);

        /* Check format */
        if (format != testcase->format) {
            printf("\nfilepath : %s\n"
                   "expected format : %d\n"
                   "actual format   : %d\n",
                   testcase->filepath,
                   testcase->format,
                   format);
        }
        UTEST_COMPARE_UINT(testcase->format, format);

        /* Check error code */
        if (err != testcase->errorcode) {
            printf("\nfilepath : %s\n"
                   "expected error : 0x%x\n"
                   "actual error   : 0x%x\n",
                   testcase->filepath,
                   testcase->errorcode,
                   err);
        }
        UTEST_COMPARE_UINT(testcase->errorcode, err);

        /* Check solid name */
        if (testcase->format == GMIO_STL_FORMAT_ASCII) {
            const char* testcase_solid_name =
                    testcase->solid_name != NULL ? testcase->solid_name : "";
            if (strcmp(result.solid_name, testcase_solid_name) != 0) {
                printf("\nfilepath : %s\n"
                       "expected solidname : %s\n"
                       "actual solidname   : %s\n",
                       testcase->filepath,
                       testcase_solid_name,
                       result.solid_name);
            }
            UTEST_COMPARE_CSTR(testcase_solid_name, result.solid_name);
        }

        ++testcase;
    }

    return NULL;
}

static const char* test_stlb_read()
{
    /* This file contains only a header and facet count(100) but no triangles */
    FILE* file = fopen(filepath_stlb_header_nofacets, "rb");
    if (file != NULL) {
        struct gmio_stream stream = gmio_stream_stdio(file);
        const int error =
                gmio_stlb_read(&stream, NULL, GMIO_ENDIANNESS_LITTLE, NULL);
        fclose(file);
        UTEST_COMPARE_INT(GMIO_STL_ERROR_FACET_COUNT, error);
    }
    else {
        UTEST_FAIL("file is NULL");
    }
    return NULL;
}

static const char* test_stlb_header_write()
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
        error = gmio_stlb_header_write(
                    &stream, GMIO_ENDIANNESS_LITTLE, &header, 0);
        fclose(outfile);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
    }

    {
        struct gmio_stl_data data = {0};
        struct gmio_stl_mesh_creator creator = gmio_stl_data_mesh_creator(&data);
        error = gmio_stl_read_file(filepath, &creator, NULL);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
        UTEST_ASSERT(gmio_stlb_header_equal(&header, &data.header));
        UTEST_COMPARE_UINT(0, data.tri_array.count);
    }

    return NULL;
}

/* Safely closes the two files \p f1 and \p f2 */
static void __tstl__fclose_2(FILE* f1, FILE* f2)
{
    if (f1 != NULL)
        fclose(f1);
    if (f2 != NULL)
        fclose(f2);
}

static const char* test_stlb_write()
{
    const char* model_fpath = filepath_stlb_grabcad_arm11;
    const char* model_fpath_out = "temp/solid.le_stlb";
    const char* model_fpath_out_be = "temp/solid.be_stlb";
    struct gmio_stl_data data = {0};
    int error = GMIO_ERROR_OK;

    /* Read input model file */
    {
        struct gmio_stl_mesh_creator creator = gmio_stl_data_mesh_creator(&data);
        error = gmio_stl_read_file(model_fpath, &creator, NULL);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
    }

    /* Write back input model file
     * Write also the model file in big-endian STL format
     */
    {
        const struct gmio_stl_mesh mesh = gmio_stl_data_mesh(&data);
        struct gmio_stl_write_options opts = {0};
        opts.stlb_header = data.header;
        error = gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_LE, model_fpath_out, &mesh, &opts);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);

        /* Big-endian version */
        error = gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_BE, model_fpath_out_be, &mesh, &opts);
    }

    /* Check input and output models are equal */
    {
        uint8_t buffer_in[2048] = {0};
        uint8_t buffer_out[2048] = {0};
        const size_t buff_size = 2048;
        size_t bytes_read_in = 0;
        size_t bytes_read_out = 0;
        FILE* in = fopen(model_fpath, "rb");
        FILE* out = fopen(model_fpath_out, "rb");
        if (in == NULL || out == NULL) {
            __tstl__fclose_2(in, out);
            perror("test_stlb_write()");
            UTEST_FAIL("fopen() error for in/out model files");
        }
        do {
            bytes_read_in = fread(buffer_in, 1, buff_size, in);
            bytes_read_out = fread(buffer_out, 1, buff_size, out);
            if (bytes_read_in != bytes_read_out) {
                __tstl__fclose_2(in, out);
                UTEST_FAIL("Different byte count between in/out");
            }
            if (memcmp(buffer_in, buffer_out, buff_size) != 0) {
                __tstl__fclose_2(in, out);
                UTEST_FAIL("Different buffer contents between in/out");
            }
        } while (!feof(in) && !feof(out)
                 && bytes_read_in > 0 && bytes_read_out > 0);
        __tstl__fclose_2(in, out);
    }

    /* Check output LE/BE models are equal */
    {
        struct gmio_stl_data data_be = {0};
        struct gmio_stl_mesh_creator creator = gmio_stl_data_mesh_creator(&data_be);
        error = gmio_stl_read_file(model_fpath_out_be, &creator, NULL);
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

static const char* test_stla_write()
{
    const char* model_filepath = filepath_stlb_grabcad_arm11;
    const char* model_filepath_out = "temp/solid.stla";
    struct gmio_stl_data data = {0}; /* TODO: fix memory leak on error */
    char header_str[GMIO_STLB_HEADER_SIZE + 1] = {0};
    int error = GMIO_ERROR_OK;

    /* Read input model file */
    {
        struct gmio_stl_mesh_creator creator = gmio_stl_data_mesh_creator(&data);
        error = gmio_stl_read_file(model_filepath, &creator, NULL);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
    }

    /* Write the model to STL ascii format */
    {
        struct gmio_stl_write_options opts = {0};
        const struct gmio_stl_mesh mesh = gmio_stl_data_mesh(&data);
        gmio_stlb_header_to_printable_str(&data.header, header_str, '_');
        opts.stla_solid_name = header_str;
        opts.stla_float32_prec = 7;
        opts.stla_float32_format = GMIO_FLOAT_TEXT_FORMAT_SHORTEST_LOWERCASE;
        error = gmio_stl_write_file(
                    GMIO_STL_FORMAT_ASCII, model_filepath_out, &mesh, &opts);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
    }

    /* Read the output STL ascii model */
    {
        char trim_header_str[sizeof(header_str)] = {0};
        struct gmio_stl_data data_stla = {0};
        struct gmio_stl_mesh_creator creator =
                gmio_stl_data_mesh_creator(&data_stla);
        size_t i = 0;
        gmio_cstr_copy(
                    trim_header_str,
                    sizeof(trim_header_str),
                    header_str,
                    sizeof(header_str));
        gmio_string_trim_from_end(trim_header_str, sizeof(header_str));
        error = gmio_stl_read_file(model_filepath_out, &creator, NULL);
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

static const char* __tstl__test_stl_read_multi_solid(
        const char* filepath, unsigned expected_solid_count)
{
    FILE* infile = fopen(filepath, "rb");
    if (infile != NULL) {
        unsigned solid_count = 0;
        int error = GMIO_ERROR_OK;
        struct gmio_stream stream = gmio_stream_stdio(infile);
        struct gmio_stl_read_options roptions = {0};
        struct gmio_stl_mesh_creator null_creator = {0};
        roptions.func_stla_get_streamsize = gmio_stla_infos_probe_streamsize;
        while (gmio_no_error(error) && !gmio_stream_at_end(&stream)) {
            error = gmio_stl_read(&stream, &null_creator, &roptions);
            if (gmio_no_error(error))
                ++solid_count;
        }
        fclose(infile);
        UTEST_ASSERT(gmio_no_error(error));
        UTEST_COMPARE_UINT(expected_solid_count, solid_count);
    }
    else {
        perror(NULL);
        UTEST_FAIL("");
    }
    return NULL;
}

static const char* test_stl_read_multi_solid()
{
    const char* res = NULL;
    res = __tstl__test_stl_read_multi_solid(filepath_stla_4meshs, 4);
    if (res != NULL)
        return res;
    res = __tstl__test_stl_read_multi_solid(filepath_stlb_4meshs, 4);
    return res;
}


static const char* test_stla_lc_numeric()
{
    struct gmio_stream null_stream = {0};
    const struct gmio_stl_mesh null_mesh = {0};
    struct gmio_stl_mesh_creator null_meshcreator = {0};
    int error[4] = {0};

    gmio_lc_numeric_save();
    setlocale(LC_NUMERIC, ""); /* "" -> environment's default locale */
    if (!gmio_lc_numeric_is_C()) {
        struct gmio_stl_read_options read_opts = {0};
        struct gmio_stl_write_options write_opts = {0};

        /* By default, check LC_NUMERIC */
        error[0] = gmio_stla_read(&null_stream, &null_meshcreator, NULL);
        error[1] = gmio_stl_write(
                    GMIO_STL_FORMAT_ASCII, &null_stream, &null_mesh, NULL);
        error[2] = gmio_stla_read(&null_stream, &null_meshcreator, &read_opts);
        error[3] = gmio_stl_write(
                    GMIO_STL_FORMAT_ASCII, &null_stream, &null_mesh, &write_opts);
        for (size_t i = 0; i < GMIO_ARRAY_SIZE(error); ++i) {
            UTEST_COMPARE_INT(GMIO_ERROR_BAD_LC_NUMERIC, error[i]);
        }
    }
    else {
        fprintf(stderr, "\nskip: default locale is NULL or already C/POSIX\n");
    }
    gmio_lc_numeric_restore();

    return NULL;
}

static void generate_stlb_tests_models()
{
    {
        FILE* outfile = fopen(filepath_stlb_empty, "wb");
        struct gmio_stream stream = gmio_stream_stdio(outfile);
        gmio_stlb_header_write(&stream, GMIO_ENDIANNESS_LITTLE, NULL, 0);
        fclose(outfile);
    }

    {
        const char model_fpath_le[] = "models/solid_one_facet.le_stlb";
        const char model_fpath_be[] = "models/solid_one_facet.be_stlb";
        struct gmio_stl_triangle tri = {
            { 0.f, 0.f, 1.f },  /* normal */
            { 0.f, 0.f, 0.f },  /* v1 */
            { 10.f, 0.f, 0.f }, /* v2 */
            { 5.f, 10.f, 0.f }, /* v3 */
            0                   /* attr */
        };
        struct gmio_stl_data data = {0};
        struct gmio_stl_mesh mesh = {0};

        data.tri_array.ptr = &tri;
        data.tri_array.count = 1;
        mesh = gmio_stl_data_mesh(&data);

        gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_LE, model_fpath_le, &mesh, NULL);
        gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_BE, model_fpath_be, &mesh, NULL);
    }

    {
        FILE* infile = fopen(filepath_stla_4meshs, "rb");
        FILE* outfile = fopen(filepath_stlb_4meshs, "wb");
        int read_error = GMIO_ERROR_OK;
        struct gmio_stream istream = gmio_stream_stdio(infile);
        struct gmio_stream ostream = gmio_stream_stdio(outfile);
        struct gmio_stl_read_options ropts = {0};
        ropts.func_stla_get_streamsize = gmio_stla_infos_probe_streamsize;
        while (gmio_no_error(read_error)) {
            struct gmio_stl_data data = {0};
            struct gmio_stl_mesh_creator creator = gmio_stl_data_mesh_creator(&data);
            struct gmio_stl_mesh mesh = {0};
            struct gmio_stl_write_options wopts = {0};
            read_error = gmio_stla_read(&istream, &creator, &ropts);
            mesh = gmio_stl_data_mesh(&data);
            wopts.stlb_header = gmio_stlb_header_str(data.solid_name);
            gmio_stl_write(GMIO_STL_FORMAT_BINARY_LE, &ostream, &mesh, &wopts);
            gmio_stl_triangle_array_free(&data.tri_array);
        }
        fclose(infile);
        fclose(outfile);
    }

    {
        FILE* outfile = fopen(filepath_stlb_header_nofacets, "wb");
        struct gmio_stream ostream = gmio_stream_stdio(outfile);
        const struct gmio_stlb_header header = {0};
        gmio_stlb_header_write(&ostream, GMIO_ENDIANNESS_LITTLE, &header, 100);
        fclose(outfile);
    }
}
