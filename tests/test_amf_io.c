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

struct __tamf__material
{
    double color[3];
    const char* name;
};

struct __tamf__triangle
{
    uint32_t vertex[3];
};

struct __tamf__mesh
{
    const struct gmio_vec3d* vec_vertex;
    uint32_t vertex_count;
    const struct __tamf__triangle* vec_triangle;
    uint32_t triangle_count;
};

struct __tamf__document
{
    const struct __tamf__material* vec_material;
    uint32_t material_count;
    struct __tamf__mesh mesh;
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
        break;
    case GMIO_AMF_DOCUMENT_ELEMENT_METADATA:
        break;
    }
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
        enum gmio_amf_mesh_element element,
        const struct gmio_amf_object_mesh_element_index* element_index,
        void* ptr_element)
{
    const struct __tamf__document* doc = (const struct __tamf__document*)cookie;
    switch (element) {
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

struct __tamf__document __tamf__create_doc_1()
{
    struct __tamf__document doc = {0};
    doc.vec_material = __tamf__doc_1_materials;
    doc.material_count = GMIO_ARRAY_SIZE(__tamf__doc_1_materials);
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
    doc.func_get_object_mesh = __tamf__get_object_mesh;
    doc.func_get_object_mesh_element = __tamf__get_object_mesh_element;
    doc.func_get_object_mesh_volume_triangle =
            __tamf__get_object_mesh_volume_triangle;
    doc.func_get_document_element_metadata =
            __tamf__get_document_element_metadata;
    doc.object_count = 1;
    doc.material_count = testdoc->material_count;
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
    wbuff.ptr = calloc(wbuffsize, 1);
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
    free(wbuff.ptr);
    return NULL;
}

static const char zip_entry_filename[] = "test.amf";
static const uint16_t zip_entry_filename_len = sizeof(zip_entry_filename) - 1;

static const char* __tamf__check_zip_local_file_header(
        const struct gmio_zip_local_file_header* zip_lfh)
{
    UTEST_COMPARE_UINT(
                zip_lfh->compress_method,
                GMIO_ZIP_COMPRESS_METHOD_DEFLATE);
    UTEST_ASSERT(
                (zip_lfh->general_purpose_flags &
                 GMIO_ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR)
                != 0);
    UTEST_COMPARE_UINT(
                zip_lfh->filename_len,
                zip_entry_filename_len);
    UTEST_ASSERT(strncmp(
                     zip_lfh->filename,
                     zip_entry_filename,
                     zip_entry_filename_len)
                 == 0);
    return NULL;
}

static const char* test_amf_write_doc_1_zip()
{
    static const size_t wbuffsize = 8192;
    struct gmio_rw_buffer wbuff = {0};
    wbuff.ptr = calloc(wbuffsize, 1);
    wbuff.len = wbuffsize;

    const struct __tamf__document testdoc = __tamf__create_doc_1();
    const struct gmio_amf_document doc = __tamf_create_doc(&testdoc);
    {   /* Write uncompressed */
        struct gmio_amf_write_options options = {0};
        options.float64_prec = 9;
        const int error = __tamf__write_amf(&wbuff, &doc, &options);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
    }

    const size_t source_len = wbuff.pos;
    const uint32_t crc32_amf_data = gmio_zlib_crc32(wbuff.ptr, source_len);
    uint8_t* source = calloc(source_len, 1);
    memcpy(source, wbuff.ptr, source_len);

    {   /* Write compressed(ZIP) */
        wbuff.pos = 0;
        struct gmio_amf_write_options options = {0};
        options.float64_prec = 9;
        options.compress = true;
        options.zip_entry_filename = zip_entry_filename;
        options.zip_entry_filename_len = zip_entry_filename_len;
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
        /* -- Read ZIP local file header */
        struct gmio_zip_local_file_header zip_lfh = {0};
        const size_t lfh_read_len =
                gmio_zip_read_local_file_header(&stream, &zip_lfh, &error);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
        zip_lfh.filename = (const char*)wbuff.ptr + wbuff.pos;
        const char* check_str = __tamf__check_zip_local_file_header(&zip_lfh);
        if (check_str != NULL)
            return check_str;
        /* -- Read ZIP end of central directory record */
        static const size_t end_of_central_dir_record_len = 22;
        wbuff.pos = zip_archive_len - end_of_central_dir_record_len;;
        struct gmio_zip_end_of_central_directory_record zip_eocdr = {0};
        gmio_zip_read_end_of_central_directory_record(
                    &stream, &zip_eocdr, &error);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
        UTEST_COMPARE_UINT(zip_eocdr.entry_count, 1);
        /* -- Read ZIP central directory */
        wbuff.pos = zip_eocdr.start_offset;
        struct gmio_zip_central_directory_header zip_cdh = {0};
        gmio_zip_read_central_directory_header(&stream, &zip_cdh, &error);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
        UTEST_COMPARE_UINT(
                    zip_cdh.compress_method,
                    GMIO_ZIP_COMPRESS_METHOD_DEFLATE);
        UTEST_ASSERT(
                    (zip_cdh.general_purpose_flags &
                     GMIO_ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR)
                    != 0);
        UTEST_COMPARE_UINT(crc32_amf_data, zip_cdh.crc32);
        UTEST_COMPARE_UINT(
                    zip_cdh.filename_len, zip_entry_filename_len);
        UTEST_ASSERT(strncmp(
                         (const char*)wbuff.ptr + wbuff.pos,
                         zip_entry_filename,
                         zip_entry_filename_len)
                     == 0);
        /* -- Read compressed AMF data */
        const size_t pos_start_amf_data =
                lfh_read_len + zip_lfh.filename_len + zip_lfh.extrafield_len;
        wbuff.pos = pos_start_amf_data;
        {
            uint8_t* dest = calloc(zip_cdh.uncompressed_size, 1);
            size_t dest_len = zip_cdh.uncompressed_size;
            const int error =
                    gmio_zlib_uncompress_buffer(
                        dest,
                        &dest_len,
                        (const uint8_t*)wbuff.ptr + wbuff.pos,
                        zip_cdh.compressed_size);
            printf("\n-- Info: z_len=%i  src_len=%i\n",
                   zip_cdh.compressed_size, source_len);
            UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
            UTEST_COMPARE_UINT(source_len, dest_len);
            UTEST_COMPARE_UINT(dest_len, zip_cdh.uncompressed_size);
            UTEST_COMPARE_INT(memcmp(dest, source, source_len), 0);
            const uint32_t crc32_uncomp = gmio_zlib_crc32(dest, dest_len);
            UTEST_COMPARE_UINT(crc32_amf_data, crc32_uncomp);
            free(dest);
        }
        /* -- Read ZIP data descriptor */
        wbuff.pos = pos_start_amf_data + zip_cdh.compressed_size;
        struct gmio_zip_data_descriptor zip_dd = {0};
        gmio_zip_read_data_descriptor(&stream, &zip_dd, &error);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
        UTEST_COMPARE_UINT(zip_dd.crc32, crc32_amf_data);
        UTEST_COMPARE_UINT(
                    zip_dd.compressed_size, zip_cdh.compressed_size);
        UTEST_COMPARE_UINT(
                    zip_dd.uncompressed_size, zip_cdh.uncompressed_size);
    }

    free(source);
    free(wbuff.ptr);
    return NULL;
}

static const char* test_amf_write_doc_1_zip64()
{
    static const size_t wbuffsize = 8192;
    struct gmio_rw_buffer wbuff = {0};
    wbuff.ptr = calloc(wbuffsize, 1);
    wbuff.len = wbuffsize;

    const struct __tamf__document testdoc = __tamf__create_doc_1();
    const struct gmio_amf_document doc = __tamf_create_doc(&testdoc);
    {   /* Write uncompressed */
        struct gmio_amf_write_options options = {0};
        options.float64_prec = 9;
        const int error = __tamf__write_amf(&wbuff, &doc, &options);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
    }

    const size_t source_len = wbuff.pos;
    const uint32_t crc32_amf_data = gmio_zlib_crc32(wbuff.ptr, source_len);
    uint8_t* source = calloc(source_len, 1);
    memcpy(source, wbuff.ptr, source_len);

    static const char zip_entry_filename[] = "test.amf";
    static const uint16_t zip_entry_filename_len = sizeof(zip_entry_filename) - 1;
    {   /* Write compressed(Zip64) */
        wbuff.pos = 0;
        struct gmio_amf_write_options options = {0};
        options.float64_prec = 9;
        options.compress = true;
        options.zip_entry_filename = zip_entry_filename;
        options.zip_entry_filename_len = zip_entry_filename_len;
        options.force_zip64_format = true;
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
        /* -- Read ZIP local file header */
        struct gmio_zip_local_file_header zip_lfh = {0};
        const size_t lfh_read_len =
                gmio_zip_read_local_file_header(&stream, &zip_lfh, &error);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
        zip_lfh.filename = (const char*)wbuff.ptr + wbuff.pos;
        const char* check_str = __tamf__check_zip_local_file_header(&zip_lfh);
        if (check_str != NULL)
            return check_str;
        /* -- Read ZIP end of central directory record */
        static const size_t end_of_central_dir_record_len = 22;
        wbuff.pos = zip_archive_len - end_of_central_dir_record_len;;
        struct gmio_zip_end_of_central_directory_record zip_eocdr = {0};
        gmio_zip_read_end_of_central_directory_record(
                    &stream, &zip_eocdr, &error);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
        UTEST_COMPARE_UINT(zip_eocdr.entry_count, 0xFFFF);
        UTEST_COMPARE_UINT(zip_eocdr.central_dir_size, 0xFFFFFFFF);
        UTEST_COMPARE_UINT(zip_eocdr.start_offset, 0xFFFFFFFF);
#if 0
        /* -- Read ZIP central directory */
        wbuff.pos = zip_eocdr.start_offset_central_dir_from_disk_start_nb;
        struct gmio_zip_central_directory_header zip_cdh = {0};
        gmio_zip_read_central_directory_header(&stream, &zip_cdh, &error);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
        UTEST_COMPARE_UINT(
                    zip_cdh.compress_method,
                    GMIO_ZIP_COMPRESS_METHOD_DEFLATE);
        UTEST_ASSERT(
                    (zip_cdh.general_purpose_flags &
                     GMIO_ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR)
                    != 0);
        UTEST_COMPARE_UINT(crc32_amf_data, zip_cdh.crc32);
        UTEST_COMPARE_UINT(
                    zip_cdh.filename_len, zip_entry_filename_len);
        UTEST_ASSERT(strncmp(
                         (const char*)wbuff.ptr + wbuff.pos,
                         zip_entry_filename,
                         zip_entry_filename_len)
                     == 0);
        /* -- Read compressed AMF data */
        const size_t pos_start_amf_data =
                lfh_read_len + zip_lfh.filename_len + zip_lfh.extrafield_len;
        wbuff.pos = pos_start_amf_data;
        {
            uint8_t* dest = calloc(zip_cdh.uncompressed_size, 1);
            size_t dest_len = zip_cdh.uncompressed_size;
            const int error =
                    gmio_zlib_uncompress_buffer(
                        dest,
                        &dest_len,
                        (const uint8_t*)wbuff.ptr + wbuff.pos,
                        zip_cdh.compressed_size);
            printf("\n-- Info: z_len=%i  src_len=%i\n",
                   zip_cdh.compressed_size, source_len);
            UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
            UTEST_COMPARE_UINT(source_len, dest_len);
            UTEST_COMPARE_UINT(dest_len, zip_cdh.uncompressed_size);
            UTEST_COMPARE_INT(memcmp(dest, source, source_len), 0);
            const uint32_t crc32_uncomp = gmio_zlib_crc32(dest, dest_len);
            UTEST_COMPARE_UINT(crc32_amf_data, crc32_uncomp);
            free(dest);
        }
        /* -- Read ZIP data descriptor */
        wbuff.pos = pos_start_amf_data + zip_cdh.compressed_size;
        struct gmio_zip_data_descriptor zip_dd = {0};
        gmio_zip64_read_data_descriptor(&stream, &zip_dd, &error);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
        UTEST_COMPARE_UINT(zip_dd.crc32, crc32_amf_data);
        UTEST_COMPARE_UINT(
                    zip_dd.compressed_size, zip_cdh.compressed_size);
        UTEST_COMPARE_UINT(
                    zip_dd.uncompressed_size, zip_cdh.uncompressed_size);
#endif
    }

    free(source);
    free(wbuff.ptr);
    return NULL;
}
