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

#include "amf_io.h"
#include "amf_error.h"

#include "../gmio_core/error.h"
#include "../gmio_core/internal/error_check.h"
#include "../gmio_core/internal/float_format_utils.h"
#include "../gmio_core/internal/helper_memblock.h"
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/helper_task_iface.h"
#include "../gmio_core/internal/ostringstream.h"
#include "../gmio_core/internal/zip_utils.h"
#include "../gmio_core/internal/zlib_utils.h"

#include <stddef.h>
#include <zlib.h>

/* Writing(output) context */
struct gmio_amf_wcontext
{
    struct gmio_ostringstream sstream;
    const struct gmio_amf_write_options* options;
    const struct gmio_amf_document* document;
    const struct gmio_task_iface* task_iface;
    intmax_t task_progress_current;
    intmax_t task_progress_max;
    struct gmio_ostringstream_format_float f64_format;
    int error;

    /* zlib specific */
    struct gmio_memblock z_memblock;
    struct z_stream_s z_stream;
    int z_flush;
    uintmax_t z_compressed_size;
    uintmax_t z_uncompressed_size;
    uint32_t z_crc32;
};

/* Helper to set error code of the writing context */
GMIO_INLINE bool gmio_amf_wcontext_set_error(
        struct gmio_amf_wcontext* context, int error)
{
    context->error = error;
    return gmio_no_error(error);
}

/* Helper to increment the current task progress of the writing context */
GMIO_INLINE void gmio_amf_wcontext_incr_task_progress(
        struct gmio_amf_wcontext* context)
{
    ++(context->task_progress_current);
}

/* Writes double value or its formula (if any) to stream */
static void gmio_amf_write_double(
        struct gmio_amf_wcontext* context,
        double value,
        const char* value_formula)
{
    struct gmio_ostringstream* sstream = &context->sstream;
    if (value_formula == NULL || *value_formula == '\0')
        gmio_ostringstream_write_f64(sstream, value, &context->f64_format);
    else
        gmio_ostringstream_write_str(sstream, value_formula);
}

/* Writes gmio_amf_color component to stream */
static void gmio_amf_write_color_component(
        struct gmio_amf_wcontext* context,
        double value,
        const char* value_formula)
{
    struct gmio_ostringstream* sstream = &context->sstream;
    if (value_formula == NULL || *value_formula == '\0') {
        gmio_ostringstream_write_f64(sstream, value, &context->f64_format);
    }
    else  {
        gmio_ostringstream_write_xmlcdata_open(sstream);
        gmio_ostringstream_write_str(sstream, value_formula);
        gmio_ostringstream_write_xmlcdata_close(sstream);
    }
}

/* Writes gmio_amf_color to stream */
static void gmio_amf_write_color(
        struct gmio_amf_wcontext* context,
        const struct gmio_amf_color* color)
{
    struct gmio_ostringstream* sstream = &context->sstream;
    gmio_ostringstream_write_chararray(sstream, "<color><r>");
    gmio_amf_write_color_component(context, color->r, color->r_formula);
    gmio_ostringstream_write_chararray(sstream, "</r><g>");
    gmio_amf_write_color_component(context, color->g, color->g_formula);
    gmio_ostringstream_write_chararray(sstream, "</g><b>");
    gmio_amf_write_color_component(context, color->b, color->b_formula);
    gmio_ostringstream_write_chararray(sstream, "</b><a>");
    gmio_amf_write_color_component(context, color->a, color->a_formula);
    gmio_ostringstream_write_chararray(sstream, "</a></color>\n");
}

/* Writes gmio_amf_metadata to stream */
static void gmio_amf_write_metadata(
        struct gmio_ostringstream* sstream,
        const struct gmio_amf_metadata* metadata)
{
    gmio_ostringstream_write_chararray(sstream, "<metadata");
    gmio_ostringstream_write_xmlattr_str(sstream, "type", metadata->type);
    gmio_ostringstream_write_char(sstream, '>');
    gmio_ostringstream_write_str(sstream, metadata->data);
    gmio_ostringstream_write_chararray(sstream, "</metadata>\n");
}

/* Writes <amf ...> to stream */
static void gmio_amf_write_amf_begin(
        struct gmio_ostringstream* sstream,
        const struct gmio_amf_document* doc)
{
    gmio_ostringstream_write_chararray(
                sstream, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    gmio_ostringstream_write_chararray(sstream, "<amf");
    if (doc->unit != GMIO_AMF_UNIT_UNKNOWN) {
        const char* unit_str = "";
        switch (doc->unit) {
        case GMIO_AMF_UNIT_MILLIMETER:
            unit_str = "millimeter"; break;
        case GMIO_AMF_UNIT_INCH:
            unit_str = "inch"; break;
        case GMIO_AMF_UNIT_FEET:
            unit_str = "feet"; break;
        case GMIO_AMF_UNIT_METER:
            unit_str = "meter"; break;
        case GMIO_AMF_UNIT_MICRON:
            unit_str = "micron"; break;
        case GMIO_AMF_UNIT_UNKNOWN: /* Silent compiler warning */
            break;
        }
        gmio_ostringstream_write_xmlattr_str(sstream, "unit", unit_str);
    }
    gmio_ostringstream_write_xmlattr_str(sstream, "version", "1.2");
    gmio_ostringstream_write_chararray(sstream, ">\n");
}

/* Writes document metadata to stream */
static bool gmio_amf_write_root_metadata(struct gmio_amf_wcontext* context)
{
    const struct gmio_amf_document* doc = context->document;
    struct gmio_amf_metadata metadata = {0};
    for (uint32_t imeta = 0; imeta < doc->metadata_count; ++imeta) {
        doc->func_get_document_element(
                    doc->cookie,
                    GMIO_AMF_DOCUMENT_ELEMENT_METADATA, imeta, &metadata);
        gmio_amf_write_metadata(&context->sstream, &metadata);
        gmio_amf_wcontext_incr_task_progress(context);
    }
    return gmio_no_error(context->error);
}

/* Writes document materials to stream */
static bool gmio_amf_write_root_materials(struct gmio_amf_wcontext* context)
{
    const struct gmio_amf_document* doc = context->document;
    struct gmio_ostringstream* sstream = &context->sstream;
    struct gmio_amf_material material = {0};
    for (uint32_t imat = 0; imat < doc->material_count; ++imat) {
        doc->func_get_document_element(
                    doc->cookie,
                    GMIO_AMF_DOCUMENT_ELEMENT_MATERIAL, imat, &material);
        gmio_ostringstream_write_chararray(sstream, "<material");
        gmio_ostringstream_write_xmlattr_u32(sstream, "id", material.id);
        gmio_ostringstream_write_chararray(sstream, ">\n");
        /* Write material <metadata> elements */
        if (material.metadata_count > 0) {
            if (doc->func_get_document_element_metadata == NULL) {
                return gmio_amf_wcontext_set_error(
                            context,
                            GMIO_AMF_ERROR_NULL_FUNC_GET_DOCUMENT_ELEMENT_METADATA);
            }
            struct gmio_amf_metadata metadata = {0};
            for (uint32_t imeta = 0; imeta < material.metadata_count; ++imeta) {
                doc->func_get_document_element_metadata(
                            doc->cookie,
                            GMIO_AMF_DOCUMENT_ELEMENT_MATERIAL,
                            imat,
                            imeta,
                            &metadata);
                gmio_amf_write_metadata(sstream, &metadata);
            }
        }
        /* Write material <color> element */
        gmio_amf_write_color(context, &material.color);
        /* Write material <composite> elements */
        if (material.composite_count > 0) {
            if (doc->func_get_material_composite == NULL) {
                return gmio_amf_wcontext_set_error(
                            context,
                            GMIO_AMF_ERROR_NULL_FUNC_GET_MATERIAL_COMPOSITE);
            }
            struct gmio_amf_composite composite = {0};
            for (uint32_t icomp = 0; icomp < material.composite_count; ++icomp) {
                doc->func_get_material_composite(
                            doc->cookie, imat, icomp, &composite);
                gmio_ostringstream_write_chararray(sstream, "<composite");
                gmio_ostringstream_write_xmlattr_u32(
                            sstream, "materialid", composite.materialid);
                gmio_ostringstream_write_char(sstream, '>');
                gmio_ostringstream_write_xmlcdata_open(sstream);
                gmio_amf_write_double(
                            context, composite.value, composite.value_formula);
                gmio_ostringstream_write_xmlcdata_close(sstream);
                gmio_ostringstream_write_chararray(sstream, "</composite>\n");
            }
        }
        gmio_ostringstream_write_chararray(sstream, "</material>\n");
        gmio_amf_wcontext_incr_task_progress(context);
    }
    return gmio_no_error(context->error);
}

/* Write gmio_amf_texmap to stream */
static void gmio_amf_write_texmap(
        struct gmio_ostringstream* sstream,
        const struct gmio_amf_texmap* texmap)
{
    /* Write triangle <texmap ...> element */
    gmio_ostringstream_write_chararray(sstream, "<texmap");
    gmio_ostringstream_write_xmlattr_u32(sstream, "rtexid", texmap->rtexid);
    gmio_ostringstream_write_xmlattr_u32(sstream, "gtexid", texmap->gtexid);
    gmio_ostringstream_write_xmlattr_u32(sstream, "btexid", texmap->btexid);
    gmio_ostringstream_write_xmlattr_u32(sstream, "atexid", texmap->atexid);
    gmio_ostringstream_write_char(sstream, '>');
    /* Write triangle <utex> elements */
    gmio_ostringstream_write_xmlelt_f64(sstream, "utex1", texmap->utex.x);
    gmio_ostringstream_write_xmlelt_f64(sstream, "utex2", texmap->utex.y);
    gmio_ostringstream_write_xmlelt_f64(sstream, "utex3", texmap->utex.z);
    /* Write triangle <vtex> elements */
    gmio_ostringstream_write_xmlelt_f64(sstream, "vtex1", texmap->vtex.x);
    gmio_ostringstream_write_xmlelt_f64(sstream, "vtex2", texmap->vtex.y);
    gmio_ostringstream_write_xmlelt_f64(sstream, "vtex3", texmap->vtex.z);
    /* Write triangle <wtex> elements */
    if (texmap->has_wtex) {
        gmio_ostringstream_write_xmlelt_f64(sstream, "wtex1", texmap->wtex.x);
        gmio_ostringstream_write_xmlelt_f64(sstream, "wtex2", texmap->wtex.y);
        gmio_ostringstream_write_xmlelt_f64(sstream, "wtex3", texmap->wtex.z);
    }
    gmio_ostringstream_write_chararray(sstream, "</texmap>\n");
}

/* Writes gmio_amf_mesh to stream */
static bool gmio_amf_write_mesh(
        struct gmio_amf_wcontext* context,
        const struct gmio_amf_mesh* mesh,
        const struct gmio_amf_object_mesh_element_index* base_mesh_element_index)
{
    const struct gmio_amf_document* doc = context->document;
    struct gmio_ostringstream* sstream = &context->sstream;
    struct gmio_amf_object_mesh_element_index mesh_elt_index =
            *base_mesh_element_index;
    const struct gmio_ostringstream_format_float* f64_format =
            &context->f64_format;
    /* Write mesh <vertices> element */
    struct gmio_amf_vertex vertex = {0};
    gmio_ostringstream_write_chararray(sstream, "<mesh>\n<vertices>\n");
    for (uint32_t ivert = 0; ivert < mesh->vertex_count; ++ivert) {
        mesh_elt_index.value = ivert;
        doc->func_get_object_mesh_element(
                    doc->cookie,
                    GMIO_AMF_MESH_ELEMENT_VERTEX, &mesh_elt_index, &vertex);
        /* Write <coordinates> element */
        gmio_ostringstream_write_chararray(sstream, "<vertex><coordinates>");
        gmio_ostringstream_write_chararray(sstream, "<x>");
        gmio_ostringstream_write_f64(sstream, vertex.coords.x, f64_format);
        gmio_ostringstream_write_chararray(sstream, "</x><y>");
        gmio_ostringstream_write_f64(sstream, vertex.coords.y, f64_format);
        gmio_ostringstream_write_chararray(sstream, "</y><z>");
        gmio_ostringstream_write_f64(sstream, vertex.coords.z, f64_format);
        gmio_ostringstream_write_chararray(sstream, "</z></coordinates>");
        /* Write <color> element */
        if (vertex.has_color)
            gmio_amf_write_color(context, &vertex.color);
        /* Write <normal> element */
        if (vertex.has_normal) {
            gmio_ostringstream_write_chararray(sstream, "<normal><nx>");
            gmio_ostringstream_write_f64(sstream, vertex.normal.x, f64_format);
            gmio_ostringstream_write_chararray(sstream, "</nx><ny>");
            gmio_ostringstream_write_f64(sstream, vertex.normal.y, f64_format);
            gmio_ostringstream_write_chararray(sstream, "</ny><nz>");
            gmio_ostringstream_write_f64(sstream, vertex.normal.z, f64_format);
            gmio_ostringstream_write_chararray(sstream, "</nz></normal>\n");
        }
        /* Write <metadata> elements */
        if (vertex.metadata_count > 0) {
            if (doc->func_get_object_mesh_vertex_metadata == NULL) {
                return gmio_amf_wcontext_set_error(
                            context,
                            GMIO_AMF_ERROR_NULL_FUNC_GET_OBJECT_MESH_VERTEX_METADATA);
            }
            struct gmio_amf_metadata metadata = {0};
            for (uint32_t imeta = 0; imeta < vertex.metadata_count; ++imeta) {
                doc->func_get_object_mesh_vertex_metadata(
                            doc->cookie, &mesh_elt_index, imeta, &metadata);
                gmio_amf_write_metadata(sstream, &metadata);
            }
        }
        gmio_ostringstream_write_chararray(sstream, "</vertex>\n");
        gmio_amf_wcontext_incr_task_progress(context);
        if (gmio_error(context->error))
            return false;
    }
    /* Write mesh vertices <edge> elements */
    if (mesh->edge_count > 0) {
        struct gmio_amf_edge edge = {0};
        for (uint32_t iedge = 0; iedge < mesh->edge_count; ++iedge) {
            mesh_elt_index.value = iedge;
            doc->func_get_object_mesh_element(
                        doc->cookie,
                        GMIO_AMF_MESH_ELEMENT_EDGE, &mesh_elt_index, &edge);
            gmio_ostringstream_write_chararray(sstream, "<edge><v1>");
            gmio_ostringstream_write_u32(sstream, edge.v1);
            gmio_ostringstream_write_chararray(sstream, "</v1><dx1>");
            gmio_ostringstream_write_f64(sstream, edge.d1.x, f64_format);
            gmio_ostringstream_write_chararray(sstream, "</dx1><dy1>");
            gmio_ostringstream_write_f64(sstream, edge.d1.y, f64_format);
            gmio_ostringstream_write_chararray(sstream, "</dy1><dz1>");
            gmio_ostringstream_write_f64(sstream, edge.d1.z, f64_format);
            gmio_ostringstream_write_chararray(sstream, "</dz1><v2>");
            gmio_ostringstream_write_u32(sstream, edge.v2);
            gmio_ostringstream_write_chararray(sstream, "</v2><dx2>");
            gmio_ostringstream_write_f64(sstream, edge.d2.x, f64_format);
            gmio_ostringstream_write_chararray(sstream, "</dx2><dy2>");
            gmio_ostringstream_write_f64(sstream, edge.d2.y, f64_format);
            gmio_ostringstream_write_chararray(sstream, "</dy2><dz2>");
            gmio_ostringstream_write_f64(sstream, edge.d2.z, f64_format);
            gmio_ostringstream_write_chararray(sstream, "</dz2></edge>\n");
            gmio_amf_wcontext_incr_task_progress(context);
            if (gmio_error(context->error))
                return false;
        }
    }
    gmio_ostringstream_write_chararray(sstream, "</vertices>\n");
    /* Write mesh <volume> elements */
    if (mesh->volume_count > 0) {
        struct gmio_amf_volume volume = {0};
        for (uint32_t ivol = 0; ivol < mesh->volume_count; ++ivol) {
            mesh_elt_index.value = ivol;
            doc->func_get_object_mesh_element(
                        doc->cookie,
                        GMIO_AMF_MESH_ELEMENT_VOLUME, &mesh_elt_index, &volume);
            /* Write <volume ...> element begin */
            gmio_ostringstream_write_chararray(sstream, "<volume");
            gmio_ostringstream_write_xmlattr_u32(
                        sstream, "materialid", volume.materialid);
            const char* str_volume_type = "";
            switch (volume.type) {
            case GMIO_AMF_VOLUME_TYPE_OBJECT:
                str_volume_type = "object"; break;
            case GMIO_AMF_VOLUME_TYPE_SUPPORT:
                str_volume_type = "support"; break;
            }
            gmio_ostringstream_write_xmlattr_str(sstream, "type", str_volume_type);
            gmio_ostringstream_write_chararray(sstream, ">\n");
            /* Write volume <metadata> elements */
            if (volume.metadata_count > 0) {
                if (doc->func_get_object_mesh_volume_metadata == NULL) {
                    return gmio_amf_wcontext_set_error(
                                context,
                                GMIO_AMF_ERROR_NULL_FUNC_GET_OBJECT_MESH_VOLUME_METADATA);
                }
                struct gmio_amf_metadata metadata = {0};
                for (uint32_t imeta = 0; imeta < volume.metadata_count; ++imeta) {
                    doc->func_get_object_mesh_volume_metadata(
                                doc->cookie, &mesh_elt_index, imeta, &metadata);
                    gmio_amf_write_metadata(sstream, &metadata);
                }
            }
            /* Write volume <color> element */
            if (volume.has_color)
                gmio_amf_write_color(context, &volume.color);
            /* Write <triangle> elements */
            if (volume.triangle_count > 0) {
                struct gmio_amf_triangle triangle = {0};
                for (uint32_t itri = 0; itri < volume.triangle_count; ++itri) {
                    doc->func_get_object_mesh_volume_triangle(
                                doc->cookie, &mesh_elt_index, itri, &triangle);
                    gmio_ostringstream_write_chararray(sstream, "<triangle>");
                    /* Write triangle <color> element */
                    if (triangle.has_color)
                        gmio_amf_write_color(context, &triangle.color);
                    /* Write triangle <v1> <v2> <v3> elements */
                    gmio_ostringstream_write_chararray(sstream, "<v1>");
                    gmio_ostringstream_write_u32(sstream, triangle.v1);
                    gmio_ostringstream_write_chararray(sstream, "</v1><v2>");
                    gmio_ostringstream_write_u32(sstream, triangle.v2);
                    gmio_ostringstream_write_chararray(sstream, "</v2><v3>");
                    gmio_ostringstream_write_u32(sstream, triangle.v3);
                    gmio_ostringstream_write_chararray(sstream, "</v3>");
                    /* Write triangle <texmap> element */
                    if (triangle.has_texmap)
                        gmio_amf_write_texmap(sstream, &triangle.texmap);
                    gmio_ostringstream_write_chararray(sstream, "</triangle>\n");
                    gmio_amf_wcontext_incr_task_progress(context);
                    if (gmio_error(context->error))
                        return false;
                }
            }
            gmio_ostringstream_write_chararray(sstream, "</volume>\n");
        }
    }
    gmio_ostringstream_write_chararray(sstream, "</mesh>\n");
    return gmio_no_error(context->error);
}

/* Writes document objects to stream */
static bool gmio_amf_write_root_objects(struct gmio_amf_wcontext* context)
{
    const struct gmio_amf_document* doc = context->document;
    struct gmio_ostringstream* sstream = &context->sstream;
    struct gmio_amf_object object = {0};
    for (uint32_t iobj = 0; iobj < doc->object_count; ++iobj) {
        doc->func_get_document_element(
                    doc->cookie,
                    GMIO_AMF_DOCUMENT_ELEMENT_OBJECT, iobj, &object);
        /* Open object element */
        gmio_ostringstream_write_chararray(sstream, "<object");
        gmio_ostringstream_write_xmlattr_u32(sstream, "id", object.id);
        gmio_ostringstream_write_chararray(sstream, ">\n");
        /* Write metadata elements */
        if (object.metadata_count > 0) {
            if (doc->func_get_document_element_metadata == NULL) {
                return gmio_amf_wcontext_set_error(
                            context,
                            GMIO_AMF_ERROR_NULL_FUNC_GET_DOCUMENT_ELEMENT_METADATA);
            }
            struct gmio_amf_metadata metadata = {0};
            for (uint32_t imeta = 0; imeta < object.metadata_count; ++imeta) {
                doc->func_get_document_element_metadata(
                            doc->cookie,
                            GMIO_AMF_DOCUMENT_ELEMENT_OBJECT,
                            iobj,
                            imeta,
                            &metadata);
                gmio_amf_write_metadata(sstream, &metadata);
            }
        }
        /* Write color element if any */
        if (object.has_color)
            gmio_amf_write_color(context, &object.color);
        /* Write mesh elements */
        if (object.mesh_count > 0) {
            struct gmio_amf_mesh mesh = {0};
            for (uint32_t imesh = 0; imesh < object.mesh_count; ++imesh) {
                struct gmio_amf_object_mesh_element_index base_mesh_elt_index;
                doc->func_get_object_mesh(doc->cookie, iobj, imesh, &mesh);
                base_mesh_elt_index.object_index = iobj;
                base_mesh_elt_index.mesh_index = imesh;
                base_mesh_elt_index.value = 0;
                gmio_amf_write_mesh(context, &mesh, &base_mesh_elt_index);
                if (gmio_error(context->error))
                    return false;
            }
        }
        /* Close object element */
        gmio_ostringstream_write_chararray(sstream, "</object>\n");
    }
    return gmio_no_error(context->error);
}

/* Writes document objects to stream */
static bool gmio_amf_write_root_textures(struct gmio_amf_wcontext* context)
{
    const struct gmio_amf_document* doc = context->document;
    struct gmio_ostringstream* sstream = &context->sstream;
    struct gmio_amf_texture texture = {0};
    for (uint32_t itex = 0; itex < doc->texture_count; ++itex) {
        doc->func_get_document_element(
                    doc->cookie,
                    GMIO_AMF_DOCUMENT_ELEMENT_TEXTURE, itex, &texture);
        gmio_ostringstream_write_chararray(sstream, "<texture");
        gmio_ostringstream_write_xmlattr_u32(sstream, "id", texture.id);
        gmio_ostringstream_write_xmlattr_u32(sstream, "width", texture.width);
        gmio_ostringstream_write_xmlattr_u32(sstream, "height", texture.height);
        gmio_ostringstream_write_xmlattr_u32(sstream, "depth", texture.depth);
        gmio_ostringstream_write_xmlattr_str(
                    sstream, "tiled", texture.tiled ? "true" : "false");
        const char* str_texture_type = "";
        switch (texture.type) {
        case GMIO_AMF_TEXTURE_TYPE_GRAYSCALE:
            str_texture_type = "grayscale"; break;
        }
        gmio_ostringstream_write_xmlattr_str(sstream, "type", str_texture_type);
        gmio_ostringstream_write_char(sstream, '>');
        gmio_ostringstream_write_base64(
                    sstream,
                    texture.binary_data.ptr,
                    texture.binary_data.size);
        gmio_ostringstream_write_chararray(sstream, "</texture>\n");
        gmio_amf_wcontext_incr_task_progress(context);
        if (gmio_error(context->error))
            return false;
    }
    return gmio_no_error(context->error);
}

/* Writes document constellations to stream */
static bool gmio_amf_write_root_constellations(struct gmio_amf_wcontext* context)
{
    const struct gmio_amf_document* doc = context->document;
    struct gmio_ostringstream* sstream = &context->sstream;
    struct gmio_amf_constellation constellation = {0};
    for (uint32_t icons = 0; icons < doc->constellation_count; ++icons) {
        doc->func_get_document_element(
                    doc->cookie,
                    GMIO_AMF_DOCUMENT_ELEMENT_CONSTELLATION, icons, &constellation);
        gmio_ostringstream_write_chararray(sstream, "<constellation");
        gmio_ostringstream_write_xmlattr_u32(sstream, "id", constellation.id);
        gmio_ostringstream_write_chararray(sstream, ">\n");
        /* Write constellation <metadata> elements */
        if (constellation.metadata_count > 0) {
            if (doc->func_get_document_element_metadata == NULL) {
                return gmio_amf_wcontext_set_error(
                            context,
                            GMIO_AMF_ERROR_NULL_FUNC_GET_DOCUMENT_ELEMENT_METADATA);
            }
            struct gmio_amf_metadata metadata = {0};
            for (uint32_t imeta = 0; imeta < constellation.metadata_count; ++imeta) {
                doc->func_get_document_element_metadata(
                            doc->cookie,
                            GMIO_AMF_DOCUMENT_ELEMENT_CONSTELLATION,
                            icons,
                            imeta,
                            &metadata);
                gmio_amf_write_metadata(sstream, &metadata);
            }
        }
        /* Write constellation <instance> elements */
        if (constellation.instance_count > 0) {
            struct gmio_amf_instance instance = {0};
            for (uint32_t iinst = 0; iinst < constellation.instance_count; ++iinst) {
                doc->func_get_constellation_instance(
                            doc->cookie, icons, iinst, &instance);
                gmio_ostringstream_write_chararray(sstream, "<instance");
                gmio_ostringstream_write_xmlattr_u32(
                            sstream, "objectid", instance.objectid);
                gmio_ostringstream_write_char(sstream, '>');
                gmio_ostringstream_write_xmlelt_f64(
                            sstream, "deltax", instance.delta.x);
                gmio_ostringstream_write_xmlelt_f64(
                            sstream, "deltay", instance.delta.y);
                gmio_ostringstream_write_xmlelt_f64(
                            sstream, "deltaz", instance.delta.z);
                gmio_ostringstream_write_xmlelt_f64(
                            sstream, "rx", instance.rot.x);
                gmio_ostringstream_write_xmlelt_f64(
                            sstream, "ry", instance.rot.y);
                gmio_ostringstream_write_xmlelt_f64(
                            sstream, "rz", instance.rot.z);
                gmio_ostringstream_write_chararray(sstream, "</instance>\n");
                gmio_amf_wcontext_incr_task_progress(context);
                if (gmio_error(context->error))
                    return false;
            }
        }
        gmio_ostringstream_write_chararray(sstream, "</constellation>\n");
    }
    return gmio_no_error(context->error);
}

/* Returns true if internal document data are roughly accessible */
static bool gmio_amf_check_document(
        int* error, const struct gmio_amf_document* doc)
{
    if (doc == NULL) {
        *error = GMIO_AMF_ERROR_NULL_DOCUMENT;
    }
    else if (doc->func_get_document_element == NULL) {
        *error = GMIO_AMF_ERROR_NULL_FUNC_GET_DOCUMENT_ELEMENT;
    }
    else if (doc->constellation_count > 0
             && doc->func_get_constellation_instance == NULL)
    {
        *error = GMIO_AMF_ERROR_NULL_FUNC_GET_CONSTELLATION_INSTANCE;
    }
    else if (doc->func_get_object_mesh == NULL) {
        *error = GMIO_AMF_ERROR_NULL_FUNC_GET_OBJECT_MESH;
    }
    else if (doc->func_get_object_mesh_element == NULL) {
        *error = GMIO_AMF_ERROR_NULL_FUNC_GET_OBJECT_MESH_ELEMENT;
    }
    else if (doc->func_get_object_mesh_volume_triangle == NULL) {
        *error = GMIO_AMF_ERROR_NULL_FUNC_GET_OBJECT_MESH_VOLUME_TRIANGLE;
    }
    return gmio_no_error(*error);
}

/* Helper for gmio_amf_ostringstream_write() to write zlib compressed data */
static size_t gmio_amf_ostringstream_write_zlib(
        struct gmio_amf_wcontext* context,
        struct gmio_stream* stream,
        const char* ptr,
        size_t len)
{
    struct gmio_memblock* z_mblock = &context->z_memblock;
    struct z_stream_s* z_stream = &context->z_stream;
    size_t total_written_len = 0;
    int z_retcode = Z_OK;

    context->z_uncompressed_size += len;
    context->z_crc32 =
            gmio_zlib_crc32_update(context->z_crc32, (const uint8_t*)ptr, len);

    z_stream->next_in = (z_const Bytef*)ptr;
    z_stream->avail_in = len;
    /* Run zlib deflate() on input until output buffer not full
     * Finish compression when zflush == Z_FINISH */
    do {
        z_stream->next_out = z_mblock->ptr;
        z_stream->avail_out = z_mblock->size;
        z_retcode = deflate(z_stream, context->z_flush);
        /* Check state not clobbered */
        if (z_retcode == Z_STREAM_ERROR) {
            context->error = zlib_error_to_gmio_error(z_retcode);
            return total_written_len;
        }
        /* Write zlib output to stream */
        {
            const size_t z_out_len =
                    z_mblock->size - z_stream->avail_out;
            const size_t written_len =
                    gmio_stream_write_bytes(stream, z_mblock->ptr, z_out_len);
            total_written_len += written_len;
            if (written_len != z_out_len || gmio_stream_error(stream)) {
                context->error = GMIO_ERROR_STREAM;
                return total_written_len;
            }
        }
    } while (z_stream->avail_out == 0);
    /* Check all input was used */
    if (z_stream->avail_in != 0) {
        /* TODO: set more precise error */
        context->error = GMIO_ERROR_UNKNOWN;
        return total_written_len;
    }
    /* Check stream is complete */
    if (context->z_flush == Z_FINISH && z_retcode != Z_STREAM_END) {
        /* TODO: set more precise error */
        context->error = GMIO_ERROR_UNKNOWN;
        return total_written_len;
    }
    context->z_compressed_size += total_written_len;
    return total_written_len;

    /* zlib official "howto" from http://zlib.net/zlib_how.html */
#if 0
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
    /* compress until end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
        /* run deflate() on input until output buffer not full, finish
         * compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;

            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */
        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    deflateEnd(&strm);
    return Z_OK;
#endif
}

/* Function called through gmio_ostringstream::func_stream_write */
static size_t gmio_amf_ostringstream_write(
        void* cookie, struct gmio_stream* stream, const char* ptr, size_t len)
{
    struct gmio_amf_wcontext* context = (struct gmio_amf_wcontext*)cookie;
    size_t len_written = 0;
    if (gmio_no_error(context->error)) {
        if (context->options->create_zip_archive) {
            len_written =
                    gmio_amf_ostringstream_write_zlib(context, stream, ptr, len);
        }
        else {
            len_written = gmio_stream_write_bytes(stream, ptr, len);
            if (len_written != len)
                context->error = GMIO_ERROR_STREAM;
        }
        if (gmio_no_error(context->error)) {
            gmio_task_iface_handle_progress(
                        context->task_iface,
                        context->task_progress_current,
                        context->task_progress_max);
            if (gmio_task_iface_is_stop_requested(context->task_iface))
                context->error = GMIO_ERROR_TASK_STOPPED;
        }
    }
    return len_written;
}

/* Returns computation upper limit of the task progress */
static intmax_t gmio_amf_task_progress_max(const struct gmio_amf_document* doc)
{
    intmax_t progress_max = 0;
    progress_max += doc->metadata_count;
    progress_max += doc->material_count;
    progress_max += doc->texture_count;
    /* Add total object(vertex_count + edge_count + triangle_count) */
    struct gmio_amf_object object = {0};
    for (uint32_t iobj = 0; iobj < doc->object_count; ++iobj) {
        doc->func_get_document_element(
                    doc->cookie,
                    GMIO_AMF_DOCUMENT_ELEMENT_OBJECT, iobj, &object);
        struct gmio_amf_mesh mesh = {0};
        for (uint32_t imesh = 0; imesh < object.mesh_count; ++imesh) {
            doc->func_get_object_mesh(doc->cookie, iobj, imesh, &mesh);
            progress_max += mesh.vertex_count;
            progress_max += mesh.edge_count;
            struct gmio_amf_object_mesh_element_index mesh_elt_index;
            mesh_elt_index.object_index = iobj;
            mesh_elt_index.mesh_index = imesh;
            mesh_elt_index.value = 0;
            for (uint32_t ivol = 0; ivol < mesh.volume_count; ++ivol) {
                struct gmio_amf_volume volume = {0};
                mesh_elt_index.value = ivol;
                doc->func_get_object_mesh_element(
                            doc->cookie,
                            GMIO_AMF_MESH_ELEMENT_VOLUME,
                            &mesh_elt_index,
                            &volume);
                progress_max += volume.triangle_count;
            }
        }
    }
    /* Add total constellation(instance_count) */
    struct gmio_amf_constellation constellation = {0};
    for (uint32_t icons = 0; icons < doc->constellation_count; ++icons) {
        doc->func_get_document_element(
                    doc->cookie,
                    GMIO_AMF_DOCUMENT_ELEMENT_CONSTELLATION,
                    icons,
                    &constellation);
        progress_max += constellation.instance_count;
    }
    return progress_max;
}

struct gmio_zip_entry_filename {
    const char* ptr;
    uint16_t len;
};

/* Returns a non-null C string for the ZIP entry filename in options */
static struct gmio_zip_entry_filename gmio_amf_zip_entry_filename(
        const struct gmio_amf_write_options* options)
{
    static const char default_filename[] = "geometry.amf";
    const char* filename = options->zip_entry_filename;
    const bool is_empty_filename = filename == NULL || *filename == '\0';
    struct gmio_zip_entry_filename zip_filename;
    zip_filename.ptr = is_empty_filename ? default_filename : filename;
    zip_filename.len =
            is_empty_filename ?
                sizeof(default_filename) - 1 :
                options->zip_entry_filename_len;
    return zip_filename;
}

/* Writes AMF file data, plain text or compressed(ZIP)
 * This function satisfies the signature required by gmio_zip_write_single_file()
 */
static int gmio_amf_write_file_data(
        void* cookie, struct gmio_zip_data_descriptor* dd)
{
    struct gmio_amf_wcontext* context = (struct gmio_amf_wcontext*)cookie;
    struct gmio_ostringstream* sstream = &context->sstream;
    gmio_amf_write_amf_begin(sstream, context->document);
    if (!gmio_amf_write_root_metadata(context))
        return context->error;
    if (!gmio_amf_write_root_materials(context))
        return context->error;
    if (!gmio_amf_write_root_objects(context))
        return context->error;
    if (!gmio_amf_write_root_textures(context))
        return context->error;
    if (!gmio_amf_write_root_constellations(context))
        return context->error;
    if (context->options->create_zip_archive) {
        gmio_ostringstream_flush(sstream);
        context->z_flush = Z_FINISH;
    }
    gmio_ostringstream_write_chararray(sstream, "</amf>\n");
    gmio_ostringstream_flush(sstream);
    if (context->options->create_zip_archive && dd != NULL) {
        dd->crc32 = context->z_crc32;
        dd->uncompressed_size = context->z_uncompressed_size;
        dd->compressed_size = context->z_compressed_size;
    }
    return context->error;
}

int gmio_amf_write(
        struct gmio_stream* stream,
        const struct gmio_amf_document* doc,
        const struct gmio_amf_write_options* opts)
{
    static const struct gmio_amf_write_options default_write_opts = {0};
    opts = opts != NULL ? opts : &default_write_opts;

    struct gmio_amf_wcontext context = {0};
    struct gmio_memblock_helper mblock_helper =
            gmio_memblock_helper(opts != NULL ? &opts->stream_memblock : NULL);
    const struct gmio_memblock* memblock = &mblock_helper.memblock;

    /* Check validity of input parameters */
    context.error = GMIO_ERROR_OK;
    /* TODO: check stream function pointers */
    if (!gmio_check_memblock(&context.error, memblock))
        goto label_end;
    if (!gmio_amf_check_document(&context.error, doc))
        goto label_end;

    /* Initialize writing context */
    const struct gmio_string_16 f64_stdio_format =
            gmio_to_stdio_float_format(opts->float64_format, opts->float64_prec);
    context.sstream =
            gmio_ostringstream(
                *stream, gmio_string(memblock->ptr, 0, memblock->size));
    context.sstream.cookie = &context;
    context.sstream.func_stream_write = &gmio_amf_ostringstream_write;
    context.options = opts;
    context.document = doc;
    context.task_iface = &opts->task_iface;
    context.task_progress_current = 0;
    if (context.task_iface->func_handle_progress != NULL)
        context.task_progress_max += gmio_amf_task_progress_max(doc);
    context.f64_format.printf_format = f64_stdio_format.array;
    context.f64_format.text_format = opts->float64_format;
    context.f64_format.precision =
            opts->float64_prec != 0 ? opts->float64_prec : 16;

    if (opts->create_zip_archive) {
        /* Initialize internal zlib stream for compression */
        const size_t mblock_halfsize = memblock->size / 2;
        context.sstream.strbuff.capacity = mblock_halfsize;
        context.z_memblock =
                gmio_memblock(
                    (uint8_t*)memblock->ptr + mblock_halfsize,
                    mblock_halfsize,
                    NULL);
        context.z_crc32 = gmio_zlib_crc32_initial();
        context.error =
                gmio_zlib_compress_init(
                    &context.z_stream, &opts->z_compress_options);
        if (gmio_error(context.error))
            goto label_end;
        context.z_flush = Z_NO_FLUSH;
        /* Write ZIP file */
        struct gmio_zip_file_entry file_entry = {0};
        file_entry.compress_method = GMIO_ZIP_COMPRESS_METHOD_DEFLATE;
        file_entry.feature_version =
                !opts->dont_use_zip64_extensions ?
                    GMIO_ZIP_FEATURE_VERSION_FILE_ZIP64_FORMAT_EXTENSIONS :
                    GMIO_ZIP_FEATURE_VERSION_FILE_COMPRESSED_DEFLATE;
        const struct gmio_zip_entry_filename zip_entry_filename =
                gmio_amf_zip_entry_filename(opts);
        file_entry.filename = zip_entry_filename.ptr;
        file_entry.filename_len = zip_entry_filename.len;
        file_entry.cookie_func_write_file_data = &context;
        file_entry.func_write_file_data = gmio_amf_write_file_data;
        gmio_zip_write_single_file(stream, &file_entry, &context.error);
    }
    else {
        context.error = gmio_amf_write_file_data(&context, NULL);
    }

label_end:
    if (opts->create_zip_archive)
        deflateEnd(&context.z_stream);
    gmio_memblock_helper_release(&mblock_helper);
    return context.error;
}

int gmio_amf_write_file(
        const char* filepath,
        const struct gmio_amf_document* doc,
        const struct gmio_amf_write_options* opts)
{
    const bool compress = opts != NULL ? opts->create_zip_archive : false;
    FILE* file = fopen(filepath, compress ? "wb" : "w");
    if (file != NULL) {
        /* TODO: if opts->zip_entry_filename is empty then try to take the
         *       filename part of filepath */
        struct gmio_stream stream = gmio_stream_stdio(file);
        const int error = gmio_amf_write(&stream, doc, opts);
        fclose(file);
        return error;
    }
    return GMIO_ERROR_STDIO;
}
