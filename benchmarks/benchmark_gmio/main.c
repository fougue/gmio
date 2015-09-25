/****************************************************************************
** gmio benchmarks
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software provides performance benchmarks for the gmio library
** (https://github.com/fougue/gmio)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#include <gmio_core/error.h>
#include <gmio_stl/stl_io.h>

#include "../commons/benchmark_tools.h"

#include <string.h>

typedef struct my_igeom
{
    uint32_t facet_count;
} my_igeom_t;

static void dummy_process_triangle(
        void* cookie,
        uint32_t triangle_id,
        const gmio_stl_triangle_t* triangle)
{
    my_igeom_t* my_igeom = (my_igeom_t*)(cookie);
    GMIO_UNUSED(triangle_id);
    GMIO_UNUSED(triangle);
    if (my_igeom != NULL)
        ++(my_igeom->facet_count);
}

static void bmk_gmio_stl_read(const char* filepath)
{
    my_igeom_t cookie = {0};
    gmio_stl_mesh_creator_t mesh_creator = {0};
    int error = GMIO_ERROR_OK;

    mesh_creator.cookie = &cookie;
    mesh_creator.func_add_triangle = dummy_process_triangle;
    error = gmio_stl_read_file(filepath, &mesh_creator, NULL);
    if (error != GMIO_ERROR_OK)
        printf("gmio error: 0x%X\n", error);
}

static gmio_endianness_t to_byte_order(gmio_stl_format_t format)
{
    switch (format) {
    case GMIO_STL_FORMAT_BINARY_LE:
        return GMIO_ENDIANNESS_LITTLE;
    case GMIO_STL_FORMAT_BINARY_BE:
        return GMIO_ENDIANNESS_BIG;
    case GMIO_STL_FORMAT_ASCII:
    case GMIO_STL_FORMAT_UNKNOWN:
        return GMIO_ENDIANNESS_HOST;
    }
    return GMIO_ENDIANNESS_UNKNOWN;
}

enum { STL_TRIANGLE_ARRAY_SIZE = 512 };
typedef struct stl_readwrite_conv
{
    gmio_transfer_t trsf;
    gmio_stream_pos_t out_stream_pos_begin;
    gmio_stl_format_t in_format;
    gmio_stl_format_t out_format;
    gmio_stl_triangle_t triangle_array[STL_TRIANGLE_ARRAY_SIZE];
    unsigned triangle_pos;
    uint32_t total_triangle_count;
} stl_readwrite_conv_t;

static void readwrite_ascii_begin_solid(
        void* cookie, size_t stream_size, const char* solid_name)
{
    stl_readwrite_conv_t* rw_conv = (stl_readwrite_conv_t*)cookie;
    gmio_stream_t* stream = &rw_conv->trsf.stream;
    GMIO_UNUSED(stream_size);
    if (rw_conv->out_format == GMIO_STL_FORMAT_ASCII) {
        stream->func_write(stream->cookie, "solid ", 1, 6);
        stream->func_write(stream->cookie, solid_name, 1, strlen(solid_name));
        stream->func_write(stream->cookie, "\n", 1, 1);
    }
    else {
        /* For binary STL, facet count <- 0 because it cannot be known at this
         * point. Header will be correctly rewritten at the end of the read
         * procedure (in gmio_stl_mesh_creator::func_end_solid() callback)
         */
        const gmio_endianness_t byte_order = to_byte_order(rw_conv->out_format);
        gmio_stlb_write_header(stream, byte_order, NULL, 0);
    }
}

static void readwrite_binary_begin_solid(
        void* cookie, uint32_t tri_count, const gmio_stlb_header_t* header)
{
    stl_readwrite_conv_t* rw_conv = (stl_readwrite_conv_t*)cookie;
    gmio_stream_t* stream = &rw_conv->trsf.stream;
    if (rw_conv->out_format == GMIO_STL_FORMAT_ASCII) {
        stream->func_write(stream->cookie, "solid\n", 1, 6);
    }
    else {
        const gmio_endianness_t byte_order = to_byte_order(rw_conv->out_format);
        gmio_stlb_write_header(stream, byte_order, header, tri_count);
    }
}

static void readwrite_get_triangle(
        const void* cookie, uint32_t tri_id, gmio_stl_triangle_t* triangle)
{
    const gmio_stl_triangle_t* tri_array = (const gmio_stl_triangle_t*)cookie;
    *triangle = tri_array[tri_id];
}

static void stl_readwrite_flush_triangles(stl_readwrite_conv_t* rw_conv)
{
    gmio_stl_mesh_t mesh = {0};
    gmio_stl_write_options_t options = {0};
    mesh.cookie = &rw_conv->triangle_array[0];
    mesh.triangle_count = rw_conv->triangle_pos;
    mesh.func_get_triangle = &readwrite_get_triangle;
    options.stl_write_triangles_only = GMIO_TRUE;
    options.stla_float32_format = GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_LOWERCASE;
    options.stla_float32_prec = 6;
    gmio_stl_write(rw_conv->out_format, &rw_conv->trsf, &mesh, &options);
    rw_conv->triangle_pos = 0;
}

static void readwrite_add_triangle(
        void* cookie, uint32_t tri_id, const gmio_stl_triangle_t* triangle)
{
    stl_readwrite_conv_t* rw_conv = (stl_readwrite_conv_t*)cookie;
    if (rw_conv->triangle_pos >= STL_TRIANGLE_ARRAY_SIZE)
        stl_readwrite_flush_triangles(rw_conv);
    rw_conv->triangle_array[rw_conv->triangle_pos] = *triangle;
    ++(rw_conv->triangle_pos);
    rw_conv->total_triangle_count = tri_id + 1;
}

static void readwrite_end_solid(void* cookie)
{
    stl_readwrite_conv_t* rw_conv = (stl_readwrite_conv_t*)cookie;
    gmio_stream_t* stream = &rw_conv->trsf.stream;
    if (rw_conv->triangle_pos != 0)
        stl_readwrite_flush_triangles(rw_conv);
    if (rw_conv->out_format == GMIO_STL_FORMAT_ASCII) {
        stream->func_write(stream->cookie, "endsolid", 1, 8);
    }
    else if (rw_conv->in_format == GMIO_STL_FORMAT_ASCII) {
        const gmio_endianness_t byte_order = to_byte_order(rw_conv->out_format);
        /* The total facet count has to be written because it wasn't known at
         * the beginning of the read procedure */
        stream->func_set_pos(stream->cookie, &rw_conv->out_stream_pos_begin);
        gmio_stlb_write_header(
                    stream, byte_order, NULL, rw_conv->total_triangle_count);
    }
}

static void bmk_gmio_stl_readwrite_conv(const char* filepath)
{
    FILE* infile = fopen(filepath, "rb");
    FILE* outfile = fopen("_readwrite_conv.stl", "wb");
    gmio_transfer_t in_trsf = {0};
    stl_readwrite_conv_t rw_conv = {0};
    gmio_stl_mesh_creator_t mesh_creator = {0};
    int error = GMIO_ERROR_OK;

    /* rw_conv.out_format = GMIO_STL_FORMAT_BINARY_LE; */
    rw_conv.out_format = GMIO_STL_FORMAT_ASCII;

    if (infile != NULL) {
        in_trsf.memblock = gmio_memblock_malloc(512 * 1024);
        in_trsf.stream = gmio_stream_stdio(infile);
        rw_conv.in_format = gmio_stl_get_format(&in_trsf.stream);
    }
    if (outfile != NULL) {
        rw_conv.trsf.memblock = gmio_memblock_malloc(512 * 1024);
        rw_conv.trsf.stream = gmio_stream_stdio(outfile);
        rw_conv.trsf.stream.func_get_pos(
                    rw_conv.trsf.stream.cookie,
                    &rw_conv.out_stream_pos_begin);
    }

    mesh_creator.cookie = &rw_conv;
    mesh_creator.func_ascii_begin_solid = &readwrite_ascii_begin_solid;
    mesh_creator.func_binary_begin_solid = &readwrite_binary_begin_solid;
    mesh_creator.func_add_triangle = &readwrite_add_triangle;
    mesh_creator.func_end_solid = &readwrite_end_solid;

    error = gmio_stl_read(&in_trsf, &mesh_creator);

    gmio_memblock_deallocate(&in_trsf.memblock);
    gmio_memblock_deallocate(&rw_conv.trsf.memblock);

    if (error != GMIO_ERROR_OK)
        printf("gmio error: 0x%X\n", error);
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        const char* filepath = argv[1];

        /* Declare benchmarks */
        benchmark_cmp_arg_t cmp_args[] = {
            { "read_file()",
              bmk_gmio_stl_read, NULL,
              NULL, NULL },
            { "readwrite_conv()",
              bmk_gmio_stl_readwrite_conv, NULL,
              NULL, NULL },
            {0}
        };
        const size_t cmp_count =
                sizeof(cmp_args) / sizeof(benchmark_cmp_arg_t) - 1;
        benchmark_cmp_result_t cmp_res[2] = {0};
        benchmark_cmp_result_array_t res_array = {0};
        const benchmark_cmp_result_header_t header = { "gmio", NULL };

        cmp_args[0].func1_filepath = filepath;
        cmp_args[1].func1_filepath = filepath;

        res_array.ptr = &cmp_res[0];
        res_array.count = cmp_count;

        /* Execute benchmarks */
        benchmark_cmp_batch(5, &cmp_args[0], &cmp_res[0], NULL, NULL);

        /* Print results */
        benchmark_print_results(
                    BENCHMARK_PRINT_FORMAT_MARKDOWN, header, res_array);
    }
    return 0;
}
