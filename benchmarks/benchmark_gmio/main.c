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

#include <gmio_core/error.h>
#include <gmio_amf/amf_io.h>
#include <gmio_stl/stl_io.h>
#include <gmio_stl/stl_io_options.h>
#include <gmio_stl/stl_mesh.h>
#include <gmio_stl/stl_mesh_creator.h>
#include <gmio_stl/stl_infos.h>
#include <gmio_stl/stl_format.h>

#include "../commons/benchmark_tools.h"

#include <stdio.h>
#include <stdlib.h>
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
    struct gmio_stl_mesh_creator mesh_creator = {0};
    int error = GMIO_ERROR_OK;

    mesh_creator.cookie = &cookie;
    mesh_creator.func_add_triangle = dummy_process_triangle;
    error = gmio_stl_read_file(filepath, &mesh_creator, NULL);
    if (error != GMIO_ERROR_OK)
        fprintf(stderr, "gmio error: 0x%X\n", error);
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

enum { GMIO_OUTPUT_FORMAT_TAG_AMF = 0x1000 };
enum gmio_output_format {
    GMIO_OUTPUT_FORMAT_STL_ASCII = GMIO_STL_FORMAT_ASCII,
    GMIO_OUTPUT_FORMAT_STL_BINARY_LE = GMIO_STL_FORMAT_BINARY_LE,
    GMIO_OUTPUT_FORMAT_STL_BINARY_BE = GMIO_STL_FORMAT_BINARY_BE,
    GMIO_OUTPUT_FORMAT_AMF_PLAIN = GMIO_OUTPUT_FORMAT_TAG_AMF + 1,
    GMIO_OUTPUT_FORMAT_AMF_ZIP
};

struct bmk_gmio_stl_convert_params {
    const char* input_filepath;
    const char* output_filepath;
    enum gmio_output_format output_format;
};

struct gmio_stl_data {
    struct gmio_stl_triangle* vec_triangle;
    uint32_t vec_triangle_count;
};

static void stldata_get_triangle(
        const void* cookie, uint32_t tri_id, struct gmio_stl_triangle* triangle)
{
    const struct gmio_stl_data* stldata = (const struct gmio_stl_data*)cookie;
    *triangle = stldata->vec_triangle[tri_id];
}

static void stldata_add_triangle(
        void* cookie, uint32_t tri_id, const struct gmio_stl_triangle* triangle)
{
    struct gmio_stl_data* stldata = (struct gmio_stl_data*)cookie;
    stldata->vec_triangle[tri_id] = *triangle;
}

static void stldata_amf_get_document_element(
        const void* cookie,
        enum gmio_amf_document_element element,
        uint32_t element_index,
        void* ptr_element)
{
    GMIO_UNUSED(cookie);
    if (element == GMIO_AMF_DOCUMENT_ELEMENT_OBJECT) {
        struct gmio_amf_object* object = (struct gmio_amf_object*)ptr_element;
        object->id = element_index;
        object->mesh_count = 1;
    }
}

static void stldata_amf_get_object_mesh(
        const void* cookie,
        uint32_t object_index,
        uint32_t mesh_index,
        struct gmio_amf_mesh* ptr_mesh)
{
    GMIO_UNUSED(object_index);
    GMIO_UNUSED(mesh_index);
    const struct gmio_stl_data* stldata = (const struct gmio_stl_data*)cookie;
    ptr_mesh->vertex_count = stldata->vec_triangle_count * 3;
    ptr_mesh->edge_count = 0;
    ptr_mesh->volume_count = 1;
}

static void stldata_amf_get_object_mesh_element(
        const void* cookie,
        const struct gmio_amf_object_mesh_element_index* element_index,
        void* ptr_element)
{
    const struct gmio_stl_data* stldata = (const struct gmio_stl_data*)cookie;
    switch (element_index->element_type) {
    case GMIO_AMF_MESH_ELEMENT_VERTEX: {
        struct gmio_amf_vertex* dst_vertex = (struct gmio_amf_vertex*)ptr_element;
        const uint32_t vertex_id = element_index->value;
        const uint32_t facet_id = vertex_id / 3;
        const uint32_t facet_vertex_id = vertex_id % 3;
        const struct gmio_stl_triangle* tri = &stldata->vec_triangle[facet_id];
        const struct gmio_vec3f* src_vertex = &tri->v1 + facet_vertex_id;
        dst_vertex->coords.x = src_vertex->x;
        dst_vertex->coords.y = src_vertex->y;
        dst_vertex->coords.z = src_vertex->z;
        break;
    }
    case GMIO_AMF_MESH_ELEMENT_EDGE:
        break;
    case GMIO_AMF_MESH_ELEMENT_VOLUME: {
        struct gmio_amf_volume* ptr_volume = (struct gmio_amf_volume*)ptr_element;
        ptr_volume->materialid = 1;
        ptr_volume->triangle_count = stldata->vec_triangle_count;
        break;
    }
    }
}

static void stldata_amf_get_object_mesh_volume_triangle(
        const void* cookie,
        const struct gmio_amf_object_mesh_element_index* volume_index,
        uint32_t triangle_index,
        struct gmio_amf_triangle* ptr_triangle)
{
    GMIO_UNUSED(cookie);
    GMIO_UNUSED(volume_index);
    const uint32_t base_vertex_id = triangle_index*3;
    ptr_triangle->v1 = base_vertex_id + 0;
    ptr_triangle->v2 = base_vertex_id + 1;
    ptr_triangle->v3 = base_vertex_id + 2;
}

static void bmk_gmio_stl_convert(const void* gparams)
{
    const struct bmk_gmio_stl_convert_params* params =
            (const struct bmk_gmio_stl_convert_params*)gparams;

    FILE* infile = fopen(params->input_filepath, "rb");
    FILE* outfile = fopen(params->output_filepath, "wb");
    struct gmio_stream instream =
            infile != NULL ? gmio_stream_stdio(infile) : gmio_stream_null();
    struct gmio_stream outstream =
            outfile != NULL ? gmio_stream_stdio(outfile) : gmio_stream_null();

    /* Find the count of facet in the input STL file */
    struct gmio_stl_infos stlinfos = {0};
    gmio_stl_infos_probe(
                &stlinfos, &instream, GMIO_STL_INFO_FLAG_FACET_COUNT, NULL);
    /* printf("Facet count: %u\n", stlinfos.facet_count); */

    /* Create the array of STL triangles */
    struct gmio_stl_data stldata = {0};
    stldata.vec_triangle = malloc(
                sizeof(struct gmio_stl_triangle) * stlinfos.facet_count);
    stldata.vec_triangle_count = stlinfos.facet_count;

    /* Read the STL data */
    struct gmio_stl_mesh_creator stlmesh_creator = {0};
    stlmesh_creator.cookie = &stldata;
    stlmesh_creator.func_add_triangle = &stldata_add_triangle;
    int error = gmio_stl_read(&instream, &stlmesh_creator, NULL);
    if (error != GMIO_ERROR_OK) {
        fprintf(stderr, "gmio_stl_read() error: 0x%X\n", error);
        goto label_end;
    }

    /* Write the STL data */
    if (params->output_format == GMIO_STL_FORMAT_ASCII
            || params->output_format & GMIO_STL_FORMAT_TAG_BINARY)
    {
        struct gmio_stl_mesh stlmesh = {0};
        stlmesh.cookie = &stldata;
        stlmesh.triangle_count = stlinfos.facet_count;
        stlmesh.func_get_triangle = &stldata_get_triangle;
        const enum gmio_stl_format stlformat =
                (enum gmio_stl_format)params->output_format;
        error = gmio_stl_write(stlformat, &outstream, &stlmesh, NULL);
    }
    else if (params->output_format & GMIO_OUTPUT_FORMAT_TAG_AMF) {
        struct gmio_amf_document doc = {0};
        doc.cookie = &stldata;
        doc.unit = GMIO_AMF_UNIT_MILLIMETER;
        doc.object_count = 1;
        doc.func_get_document_element = &stldata_amf_get_document_element;
        doc.func_get_object_mesh = &stldata_amf_get_object_mesh;
        doc.func_get_object_mesh_element = &stldata_amf_get_object_mesh_element;
        doc.func_get_object_mesh_volume_triangle =
                &stldata_amf_get_object_mesh_volume_triangle;
        struct gmio_amf_write_options options = {0};
        options.create_zip_archive =
                params->output_format == GMIO_OUTPUT_FORMAT_AMF_ZIP;
        error = gmio_amf_write(&outstream, &doc, &options);
        gmio_memblock_deallocate(&options.stream_memblock);
    }

label_end:
    fclose(infile);
    fclose(outfile);
    free(stldata.vec_triangle);
}

static void bmk_gmio_stl_convert_ascii(const void* filepath)
{
    struct bmk_gmio_stl_convert_params params = {0};
    params.input_filepath = (const char*)filepath;
    params.output_filepath = "__converted_ascii.stl";
    params.output_format = GMIO_STL_FORMAT_ASCII;
    bmk_gmio_stl_convert(&params);
}

static void bmk_gmio_stl_convert_binary_le(const void* filepath)
{
    struct bmk_gmio_stl_convert_params params = {0};
    params.input_filepath = (const char*)filepath;
    params.output_filepath = "__converted_binary-le.stl";
    params.output_format = GMIO_STL_FORMAT_BINARY_LE;
    bmk_gmio_stl_convert(&params);
}

static void bmk_gmio_stl_convert_binary_be(const void* filepath)
{
    struct bmk_gmio_stl_convert_params params = {0};
    params.input_filepath = (const char*)filepath;
    params.output_filepath = "__converted_binary-be.stl";
    params.output_format = GMIO_STL_FORMAT_BINARY_BE;
    bmk_gmio_stl_convert(&params);
}

static void bmk_gmio_stl_convert_amf_plain(const void* filepath)
{
    struct bmk_gmio_stl_convert_params params = {0};
    params.input_filepath = (const char*)filepath;
    params.output_filepath = "__converted_plain.amf";
    params.output_format = GMIO_OUTPUT_FORMAT_AMF_PLAIN;
    bmk_gmio_stl_convert(&params);
}

static void bmk_gmio_stl_convert_amf_zip(const void* filepath)
{
    struct bmk_gmio_stl_convert_params params = {0};
    params.input_filepath = (const char*)filepath;
    params.output_filepath = "__converted_amf.zip";
    params.output_format = GMIO_OUTPUT_FORMAT_AMF_ZIP;
    bmk_gmio_stl_convert(&params);
}

void bmk_gmio_stl_infos_probe_all(const void* filepath)
{
    static bool already_exec = false;
    const char* cfilepath = (const char*)filepath;
    struct gmio_stl_infos infos = {0};

    gmio_stl_infos_probe_file(&infos, cfilepath, GMIO_STL_INFO_FLAG_ALL, NULL);
    if (!already_exec) {
        printf("stl_infos_probe(ALL)\n"
               "    File: %s\n"
               "    Size: %uKo\n"
               "    Facets: %u\n",
               cfilepath,
               (unsigned)infos.size / 1024,
               infos.facet_count);
        if (infos.format == GMIO_STL_FORMAT_ASCII)
            printf("    [STLA]Solid name: %s\n", infos.stla_solidname);
        else if (infos.format & GMIO_STL_FORMAT_TAG_BINARY)
            printf("    [STLB]Header: %80.80s\n", infos.stlb_header.data);
    }
    already_exec = true;
}

void bmk_gmio_stl_infos_probe_size(const void* filepath)
{
    static bool already_exec = false;
    const char* cfilepath = (const char*)filepath;
    struct gmio_stl_infos infos = {0};

    gmio_stl_infos_probe_file(&infos, cfilepath, GMIO_STL_INFO_FLAG_SIZE, NULL);
    if (!already_exec) {
        printf("stl_infos_probe(SIZE)\n"
               "    File: %s\n"
               "    Size: %uKo\n",
               cfilepath,
               (unsigned)infos.size / 1024);
    }
    already_exec = true;
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
            { "conv_stla()",
              bmk_gmio_stl_convert_ascii, NULL,
              NULL, NULL },
            { "conv_stlb_le()",
              bmk_gmio_stl_convert_binary_le, NULL,
              NULL, NULL },
            { "conv_stlb_be()",
              bmk_gmio_stl_convert_binary_be, NULL,
              NULL, NULL },
            { "conv_amf_plain()",
              bmk_gmio_stl_convert_amf_plain, NULL,
              NULL, NULL },
            { "conv_amf_zip()",
              bmk_gmio_stl_convert_amf_zip, NULL,
              NULL, NULL },
            { "stl_infos_probe(ALL)",
              bmk_gmio_stl_infos_probe_all, NULL,
              NULL, NULL },
            { "stl_infos_probe(size)",
              bmk_gmio_stl_infos_probe_size, NULL,
              NULL, NULL },
            {0}
        };
        const size_t cmp_count = GMIO_ARRAY_SIZE(cmp_args) - 1;
        for (size_t i = 0; i < cmp_count; ++i)
            cmp_args[i].func1_arg = filepath;

        struct benchmark_cmp_result cmp_res[GMIO_ARRAY_SIZE(cmp_args)] = {0};
        struct benchmark_cmp_result_array res_array = {0};
        const struct benchmark_cmp_result_header header = { "gmio", NULL };

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
