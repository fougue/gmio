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

static void __tamf__skip_zip_local_file_header()
{

}

static const char* test_amf_write()
{
    {
        struct gmio_stream stream = {0};
        struct gmio_amf_document doc = {0};
        const int error = gmio_amf_write(&stream, &doc, NULL);
        UTEST_ASSERT(gmio_error(error));
    }

    {
        static const size_t wbuffsize = 8192;
        struct gmio_rw_buffer wbuff = {0};
        struct gmio_stream stream = gmio_stream_buffer(&wbuff);
        struct gmio_amf_document doc = {0};
        struct gmio_amf_write_options options = {0};

        const struct __tamf__material testmaterials[] = {
            { { 1., 0., 0. }, "red" },
            { { 0., 1., 0. }, "green" },
            { { 0., 0., 1. }, "blue" },
            { { 1., 1., 1. }, "white" }
        };
        const struct gmio_vec3d testvertices[] = {
            { 0.,  0., 0.},
            { 1.,  0., 0.},
            { 1., -1., 0.},
            { 0., -1., 0.},
            { 1.,  0., 1.},
            { 1., -1., 1.},
            { 0.,  0., 1.},
            { 0., -1., 1.}
        };
        const struct __tamf__triangle testtriangles[] = {
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
        struct __tamf__document testdoc = {0};

        wbuff.ptr = calloc(wbuffsize, 1);
        wbuff.len = wbuffsize;

        testdoc.vec_material = testmaterials;
        testdoc.mesh.vec_vertex = testvertices;
        testdoc.mesh.vertex_count = GMIO_ARRAY_SIZE(testvertices);
        testdoc.mesh.vec_triangle = testtriangles;
        testdoc.mesh.triangle_count = GMIO_ARRAY_SIZE(testtriangles);

        doc.cookie = &testdoc;
        doc.unit = GMIO_AMF_UNIT_MILLIMETER;
        doc.func_get_document_element = &__tamf__get_document_element;
        doc.func_get_object_mesh = &__tamf__get_object_mesh;
        doc.func_get_object_mesh_element = &__tamf__get_object_mesh_element;
        doc.func_get_object_mesh_volume_triangle =
                &__tamf__get_object_mesh_volume_triangle;
        doc.func_get_document_element_metadata =
                &__tamf__get_document_element_metadata;
        doc.object_count = 1;
        doc.material_count = GMIO_ARRAY_SIZE(testmaterials);

        options.float64_prec = 9;

        /* Write as raw contents(uncompressed) */
        {
            const int error = gmio_amf_write(&stream, &doc, &options);
            if (gmio_error(error))
                printf("\n0x%x\n", error);
            UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
            /* printf("%s\n", wbuff.ptr); */
        }

        /* Write compressed ZIP */
        {
            const size_t source_len = wbuff.pos;
            uint8_t* source = calloc(source_len, 1);

            memcpy(source, wbuff.ptr, source_len);
            wbuff.pos = 0;
            options.compress = true;
            static const char zip_entry_filename[] = "test.amf";
            options.zip_entry_filename = zip_entry_filename;
            options.zip_entry_filename_len = sizeof(zip_entry_filename) - 1;
            const int error = gmio_amf_write(&stream, &doc, &options);
            UTEST_COMPARE_INT(error, GMIO_ERROR_OK);
#if 0
            FILE* file = fopen("output.zip", "wb");
            fwrite(wbuff.ptr, 1, wbuff.pos, file);
            fclose(file);
#endif

            /* Unzip and compare with source data */
            uint8_t* rbuff = wbuff.ptr;
            /* -- Read local file header */
            UTEST_COMPARE_UINT(gmio_decode_uint32_le(rbuff), 0x04034b50);
            rbuff += 8;
            /* -- Read compression method */
            UTEST_COMPARE_UINT(
                        gmio_decode_uint16_le(rbuff),
                        GMIO_ZIP_COMPRESS_METHOD_DEFLATE);
            rbuff += 18;
            /* -- Read filename length */
            UTEST_COMPARE_UINT(
                        gmio_decode_uint16_le(rbuff),
                        options.zip_entry_filename_len);
            rbuff += 2;
            /* -- Read extrafield length */
            const uint16_t zip_extrafield_len = gmio_decode_uint16_le(rbuff);
            rbuff += 2;
            /* -- Read filename */
            UTEST_ASSERT(strncmp(
                             (const char*)rbuff,
                             options.zip_entry_filename,
                             options.zip_entry_filename_len)
                         == 0);
            rbuff += options.zip_entry_filename_len;
            /* -- Skip extrafield */
            rbuff += zip_extrafield_len;

#if 0 /* TODO: check other ZIP records, and uncompress file */
            uint8_t* dest = calloc(wbuffsize, 1);
            unsigned long dest_len = (unsigned long)wbuffsize;
            const unsigned long z_len = wbuff.pos;
            const int zerr = uncompress(dest, &dest_len, wbuff.ptr, z_len);
            printf("\n-- Info: z_len=%i  src_len=%i\n", z_len, source_len);
            UTEST_COMPARE_INT(zerr, Z_OK);
            UTEST_COMPARE_UINT(source_len, dest_len);
            UTEST_COMPARE_INT(memcmp(dest, source, source_len), 0);
            free(dest);
#endif

            free(source);
        }

        free(wbuff.ptr);
    }

    return NULL;
}
