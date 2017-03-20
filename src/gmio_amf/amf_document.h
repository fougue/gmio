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

/*! \file amf_document.h
 *  Structures for AMF document
 *
 *  \addtogroup gmio_amf
 *  @{
 */

#pragma once

#include "amf_global.h"

#include "../gmio_core/endian.h"
#include "../gmio_core/memblock.h"
#include "../gmio_core/vecgeom.h"

#include <stddef.h>

/*! Addition information about an element(eg name, description, ... */
struct gmio_amf_metadata {
    const char* type; /*!< UTF8-encoded */
    const char* data; /*!< UTF8-encoded */
};

/*! Red, green, blue and alpha channels as numbers or formulas in [0,1] */
struct gmio_amf_color {
    double r;
    double g;
    double b;
    double a;
    const char* r_formula;
    const char* g_formula;
    const char* b_formula;
    const char* a_formula;
};

struct gmio_amf_material {
    uint32_t id; /* XML:integer */
    struct gmio_amf_color color;
    uint32_t composite_count;
    uint32_t metadata_count;
};

/*! Proportion of the composition of another material
 *
 *  The proportion can be specified as a formula(with \c value_formula) or as a
 *  constant mixing(with \c value) */
struct gmio_amf_composite {
    uint32_t materialid; /* XML:nonNegativeInteger, required */
    double value; /*!< Governs the percent of material */
    const char* value_formula;
};

/*! Vertex within a mesh */
struct gmio_amf_vertex {
    struct gmio_vec3d coords;
    bool has_normal;
    struct gmio_vec3d normal; /* XML:NegOneToOne: -1 <= coord <= 1 */
    bool has_color;
    struct gmio_amf_color color; /* XML:Color */
    uint32_t metadata_count;
};

/*! Edge within a mesh, for curved triangles */
struct gmio_amf_edge {
    uint32_t v1; /* XML:nonNegativeInteger */
    uint32_t v2; /* XML:nonNegativeInteger */
    /* Direction vectors */
    struct gmio_vec3d d1; /* XML:NegOneToOne: -1 <= coord <= 1 */
    struct gmio_vec3d d2; /* XML:NegOneToOne: -1 <= coord <= 1 */
};

struct gmio_amf_texmap {
    uint32_t rtexid; /* XML:nonNegativeInteger */
    uint32_t gtexid; /* XML:nonNegativeInteger */
    uint32_t btexid; /* XML:nonNegativeInteger */
    uint32_t atexid; /* XML:nonNegativeInteger */
    struct gmio_vec3d utex; /* XML:Coords : any value */
    struct gmio_vec3d vtex; /* XML:Coords : any value */
    bool has_wtex;
    struct gmio_vec3d wtex; /* XML:Coords : any value */
};

struct gmio_amf_triangle {
    uint32_t v1; /* XML:nonNegativeInteger */
    uint32_t v2; /* XML:nonNegativeInteger */
    uint32_t v3; /* XML:nonNegativeInteger */
    bool has_texmap;
    struct gmio_amf_texmap texmap; /* XML:TexMap */
    bool has_color;
    struct gmio_amf_color color; /* XML:Color */
};

enum gmio_amf_volume_type {
    GMIO_AMF_VOLUME_TYPE_OBJECT = 0,
    GMIO_AMF_VOLUME_TYPE_SUPPORT
};

/*! Closed volume of an object */
struct gmio_amf_volume {
    uint32_t materialid; /* XML:nonNegativeInteger */
    enum gmio_amf_volume_type type;
    uint32_t triangle_count; /*!< Should be >= 4 */
    uint32_t metadata_count;
    bool has_color;
    struct gmio_amf_color color; /* XML:Color */
};

/*! Triangular mesh referencing a set of vertices */
struct gmio_amf_mesh {
    uint32_t vertex_count;
    uint32_t edge_count;
    uint32_t volume_count;
};

/*! Volume or volumes of material */
struct gmio_amf_object {
    uint32_t id; /* XML:integer */
    uint32_t mesh_count;
    uint32_t metadata_count;
    bool has_color;
    struct gmio_amf_color color; /* XML:Color */
};

/*! Combination of objects */
struct gmio_amf_constellation {
    uint32_t id; /* XML:integer */
    uint32_t instance_count; /*!< Should be >= 2 */
    uint32_t metadata_count;
};

/*! Position of an object in a constellation */
struct gmio_amf_instance
{
    uint32_t objectid; /* XML:nonNegativeInteger */
    struct gmio_vec3d delta; /* Any value */
    struct gmio_vec3d rot; /*!< Coordinates in degrees (-360 <= coord < 360) */
};

enum gmio_amf_texture_type {
    GMIO_AMF_TEXTURE_TYPE_GRAYSCALE
};

struct gmio_amf_texture {
    uint32_t id; /* XML:integer */
    uint32_t width;  /* XML:nonNegativeInteger */
    uint32_t height; /* XML:nonNegativeInteger */
    uint32_t depth;  /* XML:nonNegativeInteger */
    bool tiled;
    enum gmio_amf_texture_type type;
    struct gmio_memblock binary_data; /*!< Will be converted to base64 */
};

/*! Units supported by AMF */
enum gmio_amf_unit {
    GMIO_AMF_UNIT_UNKNOWN,
    GMIO_AMF_UNIT_MILLIMETER,
    GMIO_AMF_UNIT_INCH,
    GMIO_AMF_UNIT_FEET,
    GMIO_AMF_UNIT_METER,
    GMIO_AMF_UNIT_MICRON
};

/*! The direct elements of an AMF document(ie inside <tt><amf>...</amf></tt>) */
enum gmio_amf_document_element {
    GMIO_AMF_DOCUMENT_ELEMENT_OBJECT,
    GMIO_AMF_DOCUMENT_ELEMENT_MATERIAL,
    GMIO_AMF_DOCUMENT_ELEMENT_TEXTURE,
    GMIO_AMF_DOCUMENT_ELEMENT_CONSTELLATION,
    GMIO_AMF_DOCUMENT_ELEMENT_METADATA
};

/*! The direct elements of an AMF mesh(ie inside <tt><mesh>...</mesh></tt>) */
enum gmio_amf_mesh_element {
    GMIO_AMF_MESH_ELEMENT_VERTEX,
    GMIO_AMF_MESH_ELEMENT_EDGE,
    GMIO_AMF_MESH_ELEMENT_VOLUME
};

/*! Index of an element(vertex, edge or volume) within a mesh */
struct gmio_amf_object_mesh_element_index {
    /*! Index of the object that owns the mesh */
    uint32_t object_index;
    /*! Index of the mesh that owns element */
    uint32_t mesh_index;
    /*! Index of the inner element */
    uint32_t value;
    /*! Type of the inner element */
    enum gmio_amf_mesh_element element_type;
};

/*! Provides an interface for accessing the underlying(hidden) user AMF data
 *
 *  \note version is 1.2
 */
struct gmio_amf_document {
    /*! Opaque pointer on the user AMF document, passed as first argument to
     *  hook functions */
    const void* cookie;

    enum gmio_amf_unit unit;

    /*! Number of objects in the document, must be >= 1 */
    uint32_t object_count;

    /*! Number of materials in the document */
    uint32_t material_count;

    /*! Number of textures in the document */
    uint32_t texture_count;

    /*! Number of constellations in the document */
    uint32_t constellation_count;

    /*! Number of direct metadata attached to the document */
    uint32_t metadata_count;

    /*! Function that retrieves the i-th document sub-element
     *
     *  \p element is the type of the sub-element of interest.\n
     *  \p element_index is the index of the sub-element within the AMF
     *  document.\n
     *  The domain of this index depends on \p element :
     *   Element type | Domain of index                       | gmio type
     *  --------------|---------------------------------------|----------------
     *  OBJECT        | <tt> [0 .. object_count[        </tt> | gmio_amf_object
     *  MATERIAL      | <tt> [0 .. material_count[      </tt> | gmio_amf_material
     *  TEXTURE       | <tt> [0 .. texture_count[       </tt> | gmio_amf_texture
     *  CONSTELLATION | <tt> [0 .. constellation_count[ </tt> | gmio_amf_constellation
     *  METADATA      | <tt> [0 .. metadata_count[      </tt> | gmio_amf_metadata
     */
    void (*func_get_document_element)(
            const void* cookie,
            enum gmio_amf_document_element element,
            uint32_t element_index,
            void* ptr_element);

    /*! Optional function that retrieves the i-th \c composite within a
     *  \c material element
     *
     *  Function not required(can be set to \c NULL) if there is no material
     *  composite in the document */
    void (*func_get_material_composite)(
            const void* cookie,
            uint32_t material_index,
            uint32_t composite_index,
            struct gmio_amf_composite* ptr_composite);

    /*! Optional function that retrieves the i-th \c instance within a
     *  \c constellation element
     *
     *  Function not required(can be set to \c NULL) if there is no
     *  constellation in the document */
    void (*func_get_constellation_instance)(
            const void* cookie,
            uint32_t constellation_index,
            uint32_t instance_index,
            struct gmio_amf_instance* ptr_instance);

    /*! Function that retrieves the i-th \c mesh within an \c object element */
    void (*func_get_object_mesh)(
            const void* cookie,
            uint32_t object_index,
            uint32_t mesh_index,
            struct gmio_amf_mesh* ptr_mesh);

    /*! Function that retrieves the i-th sub-element of a \c mesh element
     *
     *  \p element_index is the index of the sub-element within the \c mesh
     *  element.\n
     *  The domain of this index(ie. \c value field) depends on \p element :
     *   Element type | Domain of index                               | gmio type
     *  --------------|-----------------------------------------------|----------------
     *  VERTEX        | <tt> [0 .. gmio_amf_mesh::vertex_count[ </tt> | gmio_amf_vertex
     *  EDGE          | <tt> [0 .. gmio_amf_mesh::edge_count[   </tt> | gmio_amf_edge
     *  VOLUME        | <tt> [0 .. gmio_amf_mesh::volume_count[ </tt> | gmio_amf_volume
     */
    void (*func_get_object_mesh_element)(
            const void* cookie,
            const struct gmio_amf_object_mesh_element_index* element_index,
            void* ptr_element);

    /*! Function that retrieves the i-th \c triangle within a mesh \c volume */
    void (*func_get_object_mesh_volume_triangle)(
            const void* cookie,
            const struct gmio_amf_object_mesh_element_index* volume_index,
            uint32_t triangle_index,
            struct gmio_amf_triangle* ptr_triangle);

    /* Function pointers to retrieve metadata */

    /*! Optional function that retrieves the i-th metadata assigned to a
     *  document sub-element
     *
     *  \p element is the type of the sub-element of interest.\n
     *  \p element_index is the index of the sub-element within the AMF
     *  document.\n
     *  The domain of this index depends on \p element :
     *   Element type | Domain of index                       | gmio type
     *  --------------|---------------------------------------|----------------
     *  MATERIAL      | <tt> [0 .. material_count[      </tt> | gmio_amf_material
     *  OBJECT        | <tt> [0 .. object_count[        </tt> | gmio_amf_object
     *  CONSTELLATION | <tt> [0 .. constellation_count[ </tt> | gmio_amf_constellation
     *
     *  Function not required(can be set to \c NULL) if there is no metadata in
     *  the document */
    void (*func_get_document_element_metadata)(
            const void* cookie,
            enum gmio_amf_document_element element,
            uint32_t element_index,
            uint32_t metadata_index,
            struct gmio_amf_metadata* ptr_metadata);

    /*! Optional function that retrieves the i-th metadata attached to a mesh
     *  element(only vertex or volume)
     *
     *  Function not required(can be set to \c NULL) if there is no metadata
     *  for all mesh elements */
    void (*func_get_object_mesh_element_metadata)(
            const void* cookie,
            const struct gmio_amf_object_mesh_element_index* mesh_element_index,
            uint32_t metadata_index,
            struct gmio_amf_metadata* ptr_metadata);
};

/*! @} */
