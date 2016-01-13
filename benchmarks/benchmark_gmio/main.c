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
#include <gmio_core/rwargs.h>
#include <gmio_stl/stl_io.h>
#include <gmio_stl/stl_io_options.h>
#include <gmio_stl/stl_mesh.h>
#include <gmio_stl/stl_mesh_creator.h>
#include <gmio_stl/stl_infos.h>
#include <gmio_stl/stl_format.h>

#include "../commons/benchmark_tools.h"

#include <string.h>

struct gmio_stl_triangle;

struct my_igeom
{
    uint32_t facet_count;
};

static void dummy_process_triangle(
        void* cookie,
        uint32_t triangle_id,
        const struct gmio_stl_triangle* triangle)
{
    struct my_igeom* my_igeom = (struct my_igeom*)(cookie);
    GMIO_UNUSED(triangle_id);
    GMIO_UNUSED(triangle);
    if (my_igeom != NULL)
        ++(my_igeom->facet_count);
}

static void bmk_gmio_stl_read(const void* filepath)
{
    struct my_igeom cookie = {0};
    struct gmio_stl_read_args read = {0};
    int error = GMIO_ERROR_OK;

    read.mesh_creator.cookie = &cookie;
    read.mesh_creator.func_add_triangle = dummy_process_triangle;
    error = gmio_stl_read_file(&read, filepath);
    if (error != GMIO_ERROR_OK)
        printf("gmio error: 0x%X\n", error);
}

static enum gmio_endianness to_byte_order(enum gmio_stl_format format)
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
struct stl_readwrite_conv
{
    struct gmio_rwargs rwargs;
    struct gmio_streampos out_stream_pos_begin;
    enum gmio_stl_format in_format;
    enum gmio_stl_format out_format;
    struct gmio_stl_triangle triangle_array[STL_TRIANGLE_ARRAY_SIZE];
    unsigned triangle_pos;
    uint32_t total_triangle_count;
};

static void readwrite_ascii_begin_solid(
        void* cookie, gmio_streamsize_t stream_size, const char* solid_name)
{
    struct stl_readwrite_conv* rw_conv = (struct stl_readwrite_conv*)cookie;
    struct gmio_stream* stream = &rw_conv->rwargs.stream;
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
        const enum gmio_endianness byte_order = to_byte_order(rw_conv->out_format);
        gmio_stlb_write_header(stream, byte_order, NULL, 0);
    }
}

static void readwrite_binary_begin_solid(
        void* cookie, uint32_t tri_count, const struct gmio_stlb_header* header)
{
    struct stl_readwrite_conv* rw_conv = (struct stl_readwrite_conv*)cookie;
    struct gmio_stream* stream = &rw_conv->rwargs.stream;
    if (rw_conv->out_format == GMIO_STL_FORMAT_ASCII) {
        stream->func_write(stream->cookie, "solid\n", 1, 6);
    }
    else {
        const enum gmio_endianness byte_order = to_byte_order(rw_conv->out_format);
        gmio_stlb_write_header(stream, byte_order, header, tri_count);
    }
}

static void readwrite_get_triangle(
        const void* cookie, uint32_t tri_id, struct gmio_stl_triangle* triangle)
{
    const struct gmio_stl_triangle* tri_array =
            (const struct gmio_stl_triangle*)cookie;
    *triangle = tri_array[tri_id];
}

static void stl_readwrite_flush_triangles(struct stl_readwrite_conv* rw_conv)
{
    struct gmio_stl_write_args write = {0};
    write.core = rw_conv->rwargs;
    write.mesh.cookie = &rw_conv->triangle_array[0];
    write.mesh.triangle_count = rw_conv->triangle_pos;
    write.mesh.func_get_triangle = &readwrite_get_triangle;
    write.options.stl_write_triangles_only = GMIO_TRUE;
    write.options.stla_float32_format = GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_LOWERCASE;
    write.options.stla_float32_prec = 6;
    gmio_stl_write(&write, rw_conv->out_format);
    rw_conv->triangle_pos = 0;
}

static void readwrite_add_triangle(
        void* cookie, uint32_t tri_id, const struct gmio_stl_triangle* triangle)
{
    struct stl_readwrite_conv* rw_conv = (struct stl_readwrite_conv*)cookie;
    if (rw_conv->triangle_pos >= STL_TRIANGLE_ARRAY_SIZE)
        stl_readwrite_flush_triangles(rw_conv);
    rw_conv->triangle_array[rw_conv->triangle_pos] = *triangle;
    ++(rw_conv->triangle_pos);
    rw_conv->total_triangle_count = tri_id + 1;
}

static void readwrite_end_solid(void* cookie)
{
    struct stl_readwrite_conv* rw_conv = (struct stl_readwrite_conv*)cookie;
    struct gmio_stream* stream = &rw_conv->rwargs.stream;
    if (rw_conv->triangle_pos != 0)
        stl_readwrite_flush_triangles(rw_conv);
    if (rw_conv->out_format == GMIO_STL_FORMAT_ASCII) {
        stream->func_write(stream->cookie, "endsolid", 1, 8);
    }
    else if (rw_conv->in_format == GMIO_STL_FORMAT_ASCII) {
        const enum gmio_endianness byte_order = to_byte_order(rw_conv->out_format);
        /* The total facet count has to be written because it wasn't known at
         * the beginning of the read procedure */
        stream->func_set_pos(stream->cookie, &rw_conv->out_stream_pos_begin);
        gmio_stlb_write_header(
                    stream, byte_order, NULL, rw_conv->total_triangle_count);
    }
}

static void bmk_gmio_stl_readwrite_conv(const void* filepath)
{
    FILE* infile = fopen(filepath, "rb");
    FILE* outfile = fopen("_readwrite_conv.stl", "wb");
    struct gmio_stl_read_args read = {0};
    struct stl_readwrite_conv rw_conv = {0};
    int error = GMIO_ERROR_OK;

    /* rw_conv.out_format = GMIO_STL_FORMAT_BINARY_LE; */
    rw_conv.out_format = GMIO_STL_FORMAT_ASCII;

    if (infile != NULL) {
        read.core.stream_memblock = gmio_memblock_malloc(512 * 1024);
        read.core.stream = gmio_stream_stdio(infile);
        rw_conv.in_format = gmio_stl_get_format(&read.core.stream);
    }
    if (outfile != NULL) {
        rw_conv.rwargs.stream_memblock = gmio_memblock_malloc(512 * 1024);
        rw_conv.rwargs.stream = gmio_stream_stdio(outfile);
        rw_conv.rwargs.stream.func_get_pos(
                    rw_conv.rwargs.stream.cookie,
                    &rw_conv.out_stream_pos_begin);
    }

    read.mesh_creator.cookie = &rw_conv;
    read.mesh_creator.func_ascii_begin_solid = &readwrite_ascii_begin_solid;
    read.mesh_creator.func_binary_begin_solid = &readwrite_binary_begin_solid;
    read.mesh_creator.func_add_triangle = &readwrite_add_triangle;
    read.mesh_creator.func_end_solid = &readwrite_end_solid;

    error = gmio_stl_read(&read);

    gmio_memblock_deallocate(&read.core.stream_memblock);
    gmio_memblock_deallocate(&rw_conv.rwargs.stream_memblock);

    if (error != GMIO_ERROR_OK)
        printf("gmio error: 0x%X\n", error);

    fclose(infile);
    fclose(outfile);
}

void bmk_gmio_stl_infos_get(const void* filepath)
{
    static gmio_bool_t already_exec = GMIO_FALSE;
    FILE* file = fopen(filepath, "rb");

    if (file != NULL) {
        struct gmio_stream stream = gmio_stream_stdio(file);
        struct gmio_stl_infos_get_args args = {0};
        const enum gmio_stl_format format = gmio_stl_get_format(&stream);

        args.stream = stream;
        gmio_stl_infos_get(&args, format, GMIO_STL_INFO_FLAG_ALL);
        if (!already_exec) {
            printf("stl_infos_get()\n"
                   "    File: %s\n"
                   "    Size: %uKo\n"
                   "    Facets: %u\n",
                   (const char*)filepath,
                   args.infos.size / 1024,
                   args.infos.facet_count);
            if (format == GMIO_STL_FORMAT_ASCII) {
                printf("    [STLA]Solid name: %s\n",
                       args.infos.stla_solidname);
            }
            else if (format == GMIO_STL_FORMAT_BINARY_LE
                     || format == GMIO_STL_FORMAT_BINARY_BE)
            {
                printf("    [STLB]Header: %80.80s\n",
                       args.infos.stlb_header.data);
            }
        }
        already_exec = GMIO_TRUE;
    }

    fclose(file);
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        const char* filepath = argv[1];

        /* Declare benchmarks */
        struct benchmark_cmp_arg cmp_args[] = {
            { "read_file()",
              bmk_gmio_stl_read, NULL,
              NULL, NULL },
            { "readwrite_conv()",
              bmk_gmio_stl_readwrite_conv, NULL,
              NULL, NULL },
            { "stl_infos_get(ALL)",
              bmk_gmio_stl_infos_get, NULL,
              NULL, NULL },
            {0}
        };
        const size_t cmp_count = GMIO_ARRAY_SIZE(cmp_args) - 1;
        struct benchmark_cmp_result cmp_res[GMIO_ARRAY_SIZE(cmp_args)] = {0};
        struct benchmark_cmp_result_array res_array = {0};
        const struct benchmark_cmp_result_header header = { "gmio", NULL };

        {
            size_t i = 0;
            for (i = 0; i < cmp_count; ++i)
                cmp_args[i].func1_arg = filepath;
        }

        res_array.ptr = &cmp_res[0];
        res_array.count = cmp_count;

        /* Execute benchmarks */
        benchmark_cmp_batch(5, cmp_args, cmp_res, NULL, NULL);

        /* Print results */
        benchmark_print_results(
                    BENCHMARK_PRINT_FORMAT_MARKDOWN, header, res_array);
    }
    return 0;
}
