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

//#include <gmio_amf/amf_io.h>
#include <gmio_core/error.h>
#include <gmio_stl/stl_io.h>
#include <gmio_stl/stl_io_options.h>
#include <gmio_stl/stl_mesh.h>
#include <gmio_stl/stl_mesh_creator.h>
#include <gmio_stl/stl_infos.h>
#include <gmio_stl/stl_format.h>

#include "../commons/benchmark_tools.h"
#include "../../tests/stl_utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <fstream>

namespace gmio {

static void bmk_stl_read(const char* filepath)
{
    STL_MeshCreator mesh_creator;
    const int error = STL_read(filepath, &mesh_creator);
    if (error != Error_OK)
        std::cerr << "STL_read() error: " << std::hex << error << '\n';
}

static const unsigned OutputFormat_TagAMF = 0x1000;
enum OutputFormat {
    OutputFormat_STL_Ascii = STL_Format_Ascii,
    OutputFormat_STL_BinaryLittleEndian = STL_Format_BinaryLittleEndian,
    OutputFormat_STL_BinaryBigEndian = STL_Format_BinaryBigEndian,
    OutputFormat_AMF_Plain = OutputFormat_TagAMF + 1,
    OutputFormat_AMF_Zip
};

struct Bmk_STL_ConvertParams {
    const char* input_filepath;
    const char* output_filepath;
    OutputFormat output_format;
};

#if 0
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
#endif

static void bmk_stl_convert(const Bmk_STL_ConvertParams& params)
{
    std::ifstream infile(params.input_filepath, std::ios::in | std::ios::binary);
    std::ofstream outfile(params.output_filepath, std::ios::out | std::ios::binary);

    const FuncReadData func_read = istream_funcReadData(&infile);
    const FuncWriteData func_write = ostream_funcWriteData(&outfile);

    // Read the STL data
    STL_MeshCreatorBasic meshcreator;
    int error = STL_read(func_read, &meshcreator);
    if (error != Error_OK) {
        std::cerr << "STL_read() error: " << std::hex << error << '\n';
        return;
    }

    // Write the STL data
    if (params.output_format == OutputFormat_STL_Ascii
            || params.output_format & STL_Format_TagBinary)
    {
        const STL_MeshBasic mesh(meshcreator.triangles());
        error = STL_write(static_cast<STL_Format>(params.output_format), func_write, mesh);
    }
    else if (params.output_format & OutputFormat_TagAMF) {
#if 0
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
#endif
    }
}

static void bmk_stl_convert_ascii(const char* filepath) {
    Bmk_STL_ConvertParams params = {};
    params.input_filepath = filepath;
    params.output_filepath = "__converted_ascii.stl";
    params.output_format = OutputFormat_STL_Ascii;
    bmk_stl_convert(params);
}

static void bmk_stl_convert_binary_le(const char* filepath) {
    Bmk_STL_ConvertParams params = {};
    params.input_filepath = filepath;
    params.output_filepath = "__converted_binary-le.stl";
    params.output_format = OutputFormat_STL_BinaryLittleEndian;
    bmk_stl_convert(params);
}

static void bmk_stl_convert_binary_be(const char* filepath)
{
    Bmk_STL_ConvertParams params = {};
    params.input_filepath = filepath;
    params.output_filepath = "__converted_binary-be.stl";
    params.output_format = OutputFormat_STL_BinaryBigEndian;
    bmk_stl_convert(params);
}

static void bmk_stl_convert_amf_plain(const char* filepath)
{
#if 0
    struct bmk_gmio_stl_convert_params params = {0};
    params.input_filepath = (const char*)filepath;
    params.output_filepath = "__converted_plain.amf";
    params.output_format = GMIO_OUTPUT_FORMAT_AMF_PLAIN;
    bmk_gmio_stl_convert(&params);
#endif
}

static void bmk_stl_convert_amf_zip(const char* filepath)
{
#if 0
    struct bmk_gmio_stl_convert_params params = {0};
    params.input_filepath = (const char*)filepath;
    params.output_filepath = "__converted_amf.zip";
    params.output_format = GMIO_OUTPUT_FORMAT_AMF_ZIP;
    bmk_gmio_stl_convert(&params);
#endif
}

void bmk_stl_infos_probe_all(const char* filepath)
{
    static bool already_exec = false;

    const Result<STL_Infos> infos = STL_probeInfos(filepath, STL_InfoFlag_All);
    if (!already_exec) {
        std::cout << "STL_probeInfos(ALL)\n"
                  << "    File:   " << filepath << '\n'
                  << "    Size:   " << std::dec << (infos.data().size / 1024) << "KB\n"
                  << "    Facets: " << std::dec << infos.data().facet_count << '\n';
        if (infos.data().format == STL_Format_Ascii) {
            std::cout << "    [STL_Ascii]Solid name: "
                      << infos.data().ascii_solid_name << '\n';
        }
        else if (infos.data().format & STL_Format_TagBinary) {
            std::cout << "    [STL_Binary]Header: "
                      << std::setw(STL_BinaryHeaderSize)
                      << std::left
                      << std::setfill(' ')
                      << STL_binaryHeaderToString(infos.data().binary_header).data()
                      << '\n';
        }
    }
    already_exec = true;
}

void bmk_stl_infos_probe_size(const char* filepath)
{
    static bool already_exec = false;

    const Result<STL_Infos> infos = STL_probeInfos(filepath, STL_InfoFlag_Size);
    if (!already_exec) {
        std::cout << "STL_probeInfos(SIZE)\n"
                  << "    File: " << filepath << '\n'
                  << "    Size: " << std::dec << (infos.data().size / 1024) << "KB\n";
    }
    already_exec = true;
}

} // namespace gmio

int main(int argc, char** argv)
{
    if (argc > 1) {
        const char* filepath = argv[1];
        const gmio::Benchmark_CmpArg cmp_args[] = {
            { "read_file()", [=]{ gmio::bmk_stl_read(filepath); }, nullptr },
            { "conv_stla()", [=]{ gmio::bmk_stl_convert_ascii(filepath); }, nullptr },
            { "conv_stlb_le()", [=]{ gmio::bmk_stl_convert_binary_le(filepath); }, nullptr },
            { "conv_stlb_be()", [=]{ gmio::bmk_stl_convert_binary_be(filepath); }, nullptr },
            { "conv_amf_plain()", [=]{ gmio::bmk_stl_convert_amf_plain(filepath); }, nullptr },
            { "conv_amf_zip()", [=]{ gmio::bmk_stl_convert_amf_zip(filepath); }, nullptr },
            { "stl_infos_probe(ALL)", [=]{ gmio::bmk_stl_infos_probe_all(filepath); }, nullptr },
            { "stl_infos_probe(size)", [=]{ gmio::bmk_stl_infos_probe_size(filepath); }, nullptr }
        };
        const std::vector<gmio::Benchmark_CmpResult> results =
                gmio::Benchmark_cmpBatch(5, makeSpan(cmp_args));
        gmio::Benchmark_printResults_Markdown(
                    std::cout, { "gmio", "" }, results);
    }
    return 0;
}

