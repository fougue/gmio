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
#include "../src/gmio_core/internal/helper_stream.h"
#include "../src/gmio_amf/amf_error.h"
#include "../src/gmio_amf/amf_io.h"

#include <stddef.h>
#include <stdio.h>

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
    const struct __tamf__document* doc =
            (const struct __tamf__document*)cookie;
    GMIO_UNUSED(object_index);
    GMIO_UNUSED(mesh_index);
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
    const struct __tamf__document* doc = (const struct __tamf__document*)cookie;
    const struct __tamf__triangle* tri = &doc->mesh.vec_triangle[triangle_index];
    GMIO_UNUSED(volume_index);
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
    const struct __tamf__document* doc = (const struct __tamf__document*)cookie;
    GMIO_UNUSED(metadata_index);
    if (element == GMIO_AMF_DOCUMENT_ELEMENT_MATERIAL) {
        ptr_metadata->type = "name";
        ptr_metadata->data = doc->vec_material[element_index].name;
    }
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
        int error = GMIO_ERROR_OK;

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
        error = gmio_amf_write(&stream, &doc, &options);
        printf("\n0x%x\n", error);
        UTEST_COMPARE_INT(error, GMIO_ERROR_OK);

        printf("%s\n", wbuff.ptr);

        free(wbuff.ptr);
    }

    return NULL;
}
