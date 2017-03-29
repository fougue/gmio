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

#include "utest_assert.h"

#include "core_utils.h"
#include "stream_buffer.h"

#include "../src/gmio_core/error.h"
#include "../src/gmio_core/internal/byte_codec.h"
#include "../src/gmio_core/internal/helper_stream.h"
#include "../src/gmio_core/internal/zip_utils.h"
#include "../src/gmio_core/internal/zlib_utils.h"
#include "../src/gmio_amf/amf_error.h"
#include "../src/gmio_amf/amf_io.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

struct __tamf__material {
    double color[3];
    const char* name;
};

struct __tamf__triangle {
    uint32_t vertex[3];
};

struct __tamf__mesh {
    const struct gmio_vec3d* vec_vertex;
    uint32_t vertex_count;
    const struct __tamf__triangle* vec_triangle;
    uint32_t triangle_count;
};

struct __tamf__document {
    const struct __tamf__material* vec_material;
    uint32_t material_count;
    uint32_t instance_count;
    struct __tamf__mesh mesh;
    const struct gmio_amf_instance* vec_instance;
};

static void __tamf__get_document_element(
        const void* cookie,
        enum gmio_amf_document_element element,
        uint32_t element_index,
        void* ptr_element)
{
    const struct __tamf__document* doc =
            (const struct __tamf__document*)cookie;
    switch (element) {
    case GMIO_AMF_DOCUMENT_ELEMENT_OBJECT: {
        struct gmio_amf_object* ptr_object =
                (struct gmio_amf_object*)ptr_element;
        ptr_object->id = element_index;
        ptr_object->mesh_count = 1;
        break;
    }
    case GMIO_AMF_DOCUMENT_ELEMENT_MATERIAL: {
        struct gmio_amf_material* ptr_material =
                (struct gmio_amf_material*)ptr_element;
        const struct __tamf__material* imat = &doc->vec_material[element_index];
        ptr_material->metadata_count = 1;
        ptr_material->id = element_index;
        ptr_material->color.r = imat->color[0];
        ptr_material->color.g = imat->color[1];
        ptr_material->color.b = imat->color[2];
        break;
    }
    case GMIO_AMF_DOCUMENT_ELEMENT_TEXTURE:
        break;
    case GMIO_AMF_DOCUMENT_ELEMENT_CONSTELLATION:
        if (element_index == 0) {
            struct gmio_amf_constellation* ptr_constellation =
                    (struct gmio_amf_constellation*)ptr_element;
            ptr_constellation->id = element_index;
            ptr_constellation->instance_count = doc->instance_count;
            ptr_constellation->metadata_count = 0;
        }
        break;
    case GMIO_AMF_DOCUMENT_ELEMENT_METADATA:
        break;
    }
}

static void __tamf__get_constellation_instance(
        const void* cookie,
        uint32_t constellation_index,
        uint32_t instance_index,
        struct gmio_amf_instance* ptr_instance)
{
    const struct __tamf__document* doc =
            (const struct __tamf__document*)cookie;
    if (constellation_index == 0)
        *ptr_instance = doc->vec_instance[instance_index];
}

static void __tamf__get_object_mesh(
        const void* cookie,
        uint32_t object_index,
        uint32_t mesh_index,
        struct gmio_amf_mesh* ptr_mesh)
{
    GMIO_UNUSED(object_index);
    GMIO_UNUSED(mesh_index);
    const struct __tamf__document* doc =
            (const struct __tamf__document*)cookie;
    ptr_mesh->vertex_count = doc->mesh.vertex_count;
    ptr_mesh->edge_count = 0;
    ptr_mesh->volume_count = 1;
}

static void __tamf__get_object_mesh_element(
        const void* cookie,
        const struct gmio_amf_object_mesh_element_index* element_index,
        void* ptr_element)
{
    const struct __tamf__document* doc = (const struct __tamf__document*)cookie;
    switch (element_index->element_type) {
    case GMIO_AMF_MESH_ELEMENT_VERTEX: {
        struct gmio_amf_vertex* ptr_vertex = (struct gmio_amf_vertex*)ptr_element;
        ptr_vertex->coords = doc->mesh.vec_vertex[element_index->value];
        break;
    }
    case GMIO_AMF_MESH_ELEMENT_EDGE:
        break;
    case GMIO_AMF_MESH_ELEMENT_VOLUME: {
        struct gmio_amf_volume* ptr_volume = (struct gmio_amf_volume*)ptr_element;
        ptr_volume->materialid = 1;
        ptr_volume->triangle_count = doc->mesh.triangle_count;
        break;
    }
    }
}

static void __tamf__get_object_mesh_volume_triangle(
        const void* cookie,
        const struct gmio_amf_object_mesh_element_index* volume_index,
        uint32_t triangle_index,
        struct gmio_amf_triangle* ptr_triangle)
{
    GMIO_UNUSED(volume_index);
    const struct __tamf__document* doc = (const struct __tamf__document*)cookie;
    const struct __tamf__triangle* tri = &doc->mesh.vec_triangle[triangle_index];
    ptr_triangle->v1 = tri->vertex[0];
    ptr_triangle->v2 = tri->vertex[1];
    ptr_triangle->v3 = tri->vertex[2];
}

static void __tamf__get_document_element_metadata(
        const void* cookie,
        enum gmio_amf_document_element element,
        uint32_t element_index,
        uint32_t metadata_index,
        struct gmio_amf_metadata* ptr_metadata)
{
    GMIO_UNUSED(metadata_index);
    const struct __tamf__document* doc = (const struct __tamf__document*)cookie;
    if (element == GMIO_AMF_DOCUMENT_ELEMENT_MATERIAL) {
        ptr_metadata->type = "name";
        ptr_metadata->data = doc->vec_material[element_index].name;
    }
}

static const char* test_amf_write_doc_null()
{
    struct gmio_stream stream = {0};
    struct gmio_amf_document doc = {0};
    const int error = gmio_amf_write(&stream, &doc, NULL);
    UTEST_ASSERT(gmio_error(error));
    return NULL;
}

const struct __tamf__material __tamf__doc_1_materials[] = {
    { { 1., 0., 0. }, "red" },
    { { 0., 1., 0. }, "green" },
    { { 0., 0., 1. }, "blue" },
    { { 1., 1., 1. }, "white" }
};

const struct gmio_vec3d __tamf__doc_1_vertices[] = {
    { 0.,  0., 0.},
    { 1.,  0., 0.},
    { 1., -1., 0.},
    { 0., -1., 0.},
    { 1.,  0., 1.},
    { 1., -1., 1.},
    { 0.,  0., 1.},
    { 0., -1., 1.}
};

const struct __tamf__triangle __tamf__doc_1_triangles[] = {
    { 0, 1, 2},
    { 0, 2, 3},
    { 1, 5, 2},
    { 1, 4, 5},
    { 6, 5, 7},
    { 6, 4, 5},
    { 0, 6, 7},
    { 0, 7, 3},
    { 0, 6, 4},
    { 0, 4, 1},
    { 3, 7, 5},
    { 3, 5, 2}
};

const struct gmio_amf_instance __tamf__doc_1_instances[] = {
    { 0, {0}, {0} },
    { 1, {10, 0,  0}, { 45, 0,  0} },
    { 2, {0,  10, 0}, { 0,  45, 0} },
    { 3, {0,  0, 10}, { 0,  0,  45} },
    { 4, {10, 10, 0}, { 45, 45,  0} },
};

struct __tamf__document __tamf__create_doc_1()
{
    struct __tamf__document doc = {0};
    doc.vec_material = __tamf__doc_1_materials;
    doc.vec_instance = __tamf__doc_1_instances;
    doc.material_count = GMIO_ARRAY_SIZE(__tamf__doc_1_materials);
    doc.instance_count = GMIO_ARRAY_SIZE(__tamf__doc_1_instances);
    doc.mesh.vec_vertex = __tamf__doc_1_vertices;
    doc.mesh.vertex_count = GMIO_ARRAY_SIZE(__tamf__doc_1_vertices);
    doc.mesh.vec_triangle = __tamf__doc_1_triangles;
    doc.mesh.triangle_count = GMIO_ARRAY_SIZE(__tamf__doc_1_triangles);
    return doc;
}

struct gmio_amf_document __tamf_create_doc(
        const struct __tamf__document* testdoc)
{
    struct gmio_amf_document doc = {0};
    doc.cookie = testdoc;
    doc.unit = GMIO_AMF_UNIT_MILLIMETER;
    doc.func_get_document_element = __tamf__get_document_element;
    doc.func_get_constellation_instance = __tamf__get_constellation_instance;
    doc.func_get_object_mesh = __tamf__get_object_mesh;
    doc.func_get_object_mesh_element = __tamf__get_object_mesh_element;
    doc.func_get_object_mesh_volume_triangle =
            __tamf__get_object_mesh_volume_triangle;
    doc.func_get_document_element_metadata =
            __tamf__get_document_element_metadata;
    doc.object_count = 1;
    doc.material_count = testdoc->material_count;
    doc.constellation_count = testdoc->instance_count > 0 ? 1 : 0;
    return doc;
}

static int __tamf__write_amf(
        struct gmio_rw_buffer* wbuff,
        const struct gmio_amf_document* doc,
        const struct gmio_amf_write_options* opts)
{
    struct gmio_stream stream = gmio_stream_buffer(wbuff);
    return gmio_amf_write(&stream, doc, opts);
}

static const char* test_amf_write_doc_1_plaintext()
{
    static const size_t wbuffsize = 8192;
    struct gmio_rw_buffer wbuff = {0};
    wbuff.ptr = g_testamf_memblock.ptr;
    wbuff.len = wbuffsize;

    const struct __tamf__document testdoc = __tamf__create_doc_1();
    const struct gmio_amf_document doc = __tamf_create_doc(&testdoc);
    struct gmio_amf_write_options options = {0};
    options.float64_prec = 9;
    const int error = __tamf__write_amf(&wbuff, &doc, &options);
#if 0
    if (gmio_error(error))
        printf("\n0x%x\n", error);
#endif
    UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
    /* printf("%s\n", wbuff.ptr); */
    return NULL;
}

static const char zip_entry_filename[] = "test.amf";
static const uint16_t zip_entry_filename_len = sizeof(zip_entry_filename) - 1;

static const char* test_amf_write_doc_1_zip()
{
    static const size_t wbuffsize = 8192;
    struct gmio_rw_buffer wbuff = {0};
    uint8_t* ptr_g_memblock = g_testamf_memblock.ptr;
    wbuff.ptr = ptr_g_memblock;
    wbuff.len = wbuffsize;
    ptr_g_memblock += wbuff.len;

    const struct __tamf__document testdoc = __tamf__create_doc_1();
    const struct gmio_amf_document doc = __tamf_create_doc(&testdoc);
    {   /* Write uncompressed */
        struct gmio_amf_write_options options = {0};
        options.float64_prec = 9;
        const int error = __tamf__write_amf(&wbuff, &doc, &options);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
    }

    const size_t amf_data_len = wbuff.pos;
    const uint32_t crc32_amf_data = gmio_zlib_crc32(wbuff.ptr, amf_data_len);
    uint8_t* amf_data = ptr_g_memblock;
    memcpy(amf_data, wbuff.ptr, amf_data_len);
    ptr_g_memblock += amf_data_len;

    {   /* Write compressed(ZIP) */
        wbuff.pos = 0;
        struct gmio_amf_write_options options = {0};
        options.float64_prec = 9;
        options.create_zip_archive = true;
        options.zip_entry_filename = zip_entry_filename;
        options.zip_entry_filename_len = zip_entry_filename_len;
        options.dont_use_zip64_extensions = true;
        const int error = __tamf__write_amf(&wbuff, &doc, &options);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
#if 1
        FILE* file = fopen("output.zip", "wb");
        fwrite(wbuff.ptr, 1, wbuff.pos, file);
        fclose(file);
#endif
    }

    /* Unzip and compare with source data */
    {
        /* Total size in bytes of the ZIP archive */
        const uintmax_t zip_archive_len = wbuff.pos;
        wbuff.pos = 0;
        struct gmio_stream stream = gmio_stream_buffer(&wbuff);
        int error = GMIO_ERROR_OK;
        /* -- Read ZIP end of central directory record */
        wbuff.pos =
                zip_archive_len - GMIO_ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD;
        struct gmio_zip_end_of_central_directory_record zip_eocdr = {0};
        gmio_zip_read_end_of_central_directory_record(&stream, &zip_eocdr, &error);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
        /* -- Read ZIP central directory */
        wbuff.pos = zip_eocdr.central_dir_offset;
        struct gmio_zip_central_directory_header zip_cdh = {0};
        gmio_zip_read_central_directory_header(&stream, &zip_cdh, &error);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
        UTEST_COMPARE_UINT(amf_data_len, zip_cdh.uncompressed_size);
        const uint32_t amf_zdata_len = zip_cdh.compressed_size;
        /* -- Read(skip) ZIP local file header */
        wbuff.pos = 0;
        struct gmio_zip_local_file_header zip_lfh = {0};
        const size_t lfh_read_len =
                gmio_zip_read_local_file_header(&stream, &zip_lfh, &error);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
        /* -- Read and check compressed AMF data */
        wbuff.pos = lfh_read_len + zip_lfh.filename_len + zip_lfh.extrafield_len;
        {
            uint8_t* dest = ptr_g_memblock;
            size_t dest_len = amf_data_len;
            ptr_g_memblock += dest_len;
            const uint8_t* amf_zdata = (const uint8_t*)wbuff.ptr + wbuff.pos;
            const int error = gmio_zlib_uncompress_buffer(
                        dest, &dest_len, amf_zdata, amf_zdata_len);
            printf("\ninfo: z_len=%u  src_len=%u\n",
                   (unsigned)amf_zdata_len, (unsigned)amf_data_len);
            UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
            UTEST_COMPARE_UINT(dest_len, amf_data_len);
            UTEST_COMPARE_INT(memcmp(dest, amf_data, amf_data_len), 0);
            const uint32_t crc32_uncomp = gmio_zlib_crc32(dest, dest_len);
            UTEST_COMPARE_UINT(crc32_amf_data, crc32_uncomp);
        }
    }

    return NULL;
}

static const char* test_amf_write_doc_1_zip64()
{
    static const size_t wbuffsize = 8192;
    struct gmio_rw_buffer wbuff = {0};
    wbuff.ptr = g_testamf_memblock.ptr;
    wbuff.len = wbuffsize;

    const struct __tamf__document testdoc = __tamf__create_doc_1();
    const struct gmio_amf_document doc = __tamf_create_doc(&testdoc);
    {   /* Write uncompressed */
        struct gmio_amf_write_options options = {0};
        options.float64_prec = 9;
        const int error = __tamf__write_amf(&wbuff, &doc, &options);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
    }

    const uintmax_t amf_data_len = wbuff.pos;

    {   /* Write compressed(Zip64) */
        wbuff.pos = 0;
        struct gmio_amf_write_options options = {0};
        options.float64_prec = 9;
        options.create_zip_archive = true;
        options.zip_entry_filename = zip_entry_filename;
        options.zip_entry_filename_len = zip_entry_filename_len;
        const int error = __tamf__write_amf(&wbuff, &doc, &options);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
#if 1
        FILE* file = fopen("output_64.zip", "wb");
        fwrite(wbuff.ptr, 1, wbuff.pos, file);
        fclose(file);
#endif
    }

    /* Unzip and compare with source data */
    {
        /* Total size in bytes of the ZIP archive */
        const uintmax_t zip_archive_len = wbuff.pos;
        wbuff.pos = 0;
        struct gmio_stream stream = gmio_stream_buffer(&wbuff);
        int error = GMIO_ERROR_OK;
        /* -- Read Zip64 end of central directory record */
        wbuff.pos =
                zip_archive_len
                - GMIO_ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD
                - GMIO_ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_LOCATOR
                - GMIO_ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD;
        struct gmio_zip64_end_of_central_directory_record zip64_eocdr = {0};
        gmio_zip64_read_end_of_central_directory_record(
                    &stream, &zip64_eocdr, &error);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
        /* -- Read ZIP central directory */
        wbuff.pos = zip64_eocdr.central_dir_offset;
        struct gmio_zip_central_directory_header zip_cdh = {0};
        gmio_zip_read_central_directory_header(&stream, &zip_cdh, &error);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
        /* -- Read ZIP central directory Zip64 extrafield*/
        wbuff.pos =
                zip64_eocdr.central_dir_offset
                + GMIO_ZIP_SIZE_CENTRAL_DIRECTORY_HEADER
                + zip_cdh.filename_len;
        struct gmio_zip64_extrafield zip64_extra = {0};
        gmio_zip64_read_extrafield(&stream, &zip64_extra, &error);
        UTEST_COMPARE_INT(GMIO_ERROR_OK, error);
        UTEST_COMPARE_UINT(amf_data_len, zip64_extra.uncompressed_size);
    }

    return NULL;
}

static const char* test_amf_write_doc_1_zip64_file()
{
    const struct __tamf__document testdoc = __tamf__create_doc_1();
    const struct gmio_amf_document doc = __tamf_create_doc(&testdoc);
    struct gmio_amf_write_options options = {0};
    options.float64_prec = 9;
    options.create_zip_archive = true;
    const int error = gmio_amf_write_file("output_64_file.zip", &doc, &options);
    UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
    return NULL;
}

struct __tamf__task {
    intmax_t max_value;
    intmax_t current_value;
    intmax_t trigger_stop_value;
    bool progress_error;
};

static bool __tamf__is_stop_requested(void* cookie)
{
    struct __tamf__task* task = (struct __tamf__task*)cookie;
    return task->current_value >= task->trigger_stop_value;
}

static void __tamf__handle_progress(
        void* cookie, intmax_t value, intmax_t max_value)
{
    struct __tamf__task* task = (struct __tamf__task*)cookie;
    if (task->current_value > value)
        task->progress_error = true;
    task->current_value = value;
    task->max_value = max_value;
}

static const char* test_amf_write_doc_1_task_iface()
{
    static const size_t wbuffsize = 8192;
    struct gmio_rw_buffer wbuff = {0};
    wbuff.ptr = g_testamf_memblock.ptr;
    wbuff.len = wbuffsize;
    const struct __tamf__document testdoc = __tamf__create_doc_1();
    const struct gmio_amf_document doc = __tamf_create_doc(&testdoc);
    struct gmio_amf_write_options options = {0};
    struct __tamf__task task = {0};
    options.task_iface.cookie = &task;
    options.task_iface.func_handle_progress = __tamf__handle_progress;
    {
        const int error = __tamf__write_amf(&wbuff, &doc, &options);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
        UTEST_ASSERT(!task.progress_error);
        UTEST_COMPARE_INT(task.current_value, task.max_value);
        printf("\ninfo: max_value=%d\n", (int)task.max_value);
    }

    uint8_t memblock[256] = {0};
    options.stream_memblock = gmio_memblock(&memblock, sizeof(memblock), NULL);
    task.trigger_stop_value = task.max_value / 2;
    options.task_iface.func_is_stop_requested = __tamf__is_stop_requested;
    {
        const int error = __tamf__write_amf(&wbuff, &doc, &options);
        UTEST_COMPARE_INT(error, GMIO_ERROR_TASK_STOPPED);
        UTEST_ASSERT(task.current_value < task.max_value);
    }

    return NULL;
}
