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

/*! \file amf_document.h
 *  Structures for AMF document
 *
 *  \addtogroup gmio_amf
 *  @{
 */

#ifndef GMIO_AMF_DOCUMENT_H
#define GMIO_AMF_DOCUMENT_H

#include "amf_global.h"

#include "../gmio_core/endian.h"
#include "../gmio_core/memblock.h"
#include "../gmio_core/vecgeom.h"

#include <stddef.h>

struct gmio_amf_metadata
{
    const char* type; /* UTF8-encoded */
    const char* data; /* UTF8-encoded */
};

struct gmio_amf_color
{
    double r; /* in [0,1] */
    double g; /* in [0,1] */
    double b; /* in [0,1] */
    double a; /* in [0,1] optional */
    const char* r_formula;
    const char* g_formula;
    const char* b_formula;
    const char* a_formula;
};

struct gmio_amf_material
{
    uint32_t id; /* XML:integer */
    struct gmio_amf_color color;
    uint32_t composite_count;
    uint32_t metadata_count;
};

struct gmio_amf_composite
{
    uint32_t materialid; /* XML:nonNegativeInteger, required */
    double value; /* governs the percent of material */
    const char* value_formula;
};

struct gmio_amf_vertex
{
    struct gmio_vec3d coords;
    bool has_normal;
    struct gmio_vec3d normal; /* XML:NegOneToOne: -1 <= coord <= 1 */
    bool has_color;
    struct gmio_amf_color color; /* XML:Color */
    uint32_t metadata_count;
};

struct gmio_amf_edge
{
    uint32_t v1; /* XML:nonNegativeInteger */
    uint32_t v2; /* XML:nonNegativeInteger */
    /* Direction vectors */
    struct gmio_vec3d d1; /* XML:NegOneToOne: -1 <= coord <= 1 */
    struct gmio_vec3d d2; /* XML:NegOneToOne: -1 <= coord <= 1 */
};

struct gmio_amf_texmap
{
    uint32_t rtexid; /* XML:nonNegativeInteger */
    uint32_t gtexid; /* XML:nonNegativeInteger */
    uint32_t btexid; /* XML:nonNegativeInteger */
    uint32_t atexid; /* XML:nonNegativeInteger */
    struct gmio_vec3d utex; /* XML:Coords : any value */
    struct gmio_vec3d vtex; /* XML:Coords : any value */
    bool has_wtex;
    struct gmio_vec3d wtex; /* XML:Coords : any value */
};

struct gmio_amf_triangle
{
    uint32_t v1; /* XML:nonNegativeInteger */
    uint32_t v2; /* XML:nonNegativeInteger */
    uint32_t v3; /* XML:nonNegativeInteger */
    bool has_texmap;
    struct gmio_amf_texmap texmap; /* XML:TexMap */
    bool has_color;
    struct gmio_amf_color color; /* XML:Color */
};

enum gmio_amf_volume_type
{
    GMIO_AMF_VOLUME_TYPE_OBJECT = 0,
    GMIO_AMF_VOLUME_TYPE_SUPPORT
};

struct gmio_amf_volume
{
    uint32_t materialid; /* XML:nonNegativeInteger */
    enum gmio_amf_volume_type type;
    uint32_t triangle_count; /* Should be >= 4 */
    uint32_t metadata_count;
    bool has_color;
    struct gmio_amf_color color; /* XML:Color */
};

struct gmio_amf_mesh
{
    uint32_t vertex_count;
    uint32_t edge_count;
    uint32_t volume_count;
};

struct gmio_amf_object
{
    uint32_t id; /* XML:integer */
    uint32_t mesh_count;
    uint32_t metadata_count;
    bool has_color;
    struct gmio_amf_color color; /* XML:Color */
};

struct gmio_amf_constellation
{
    uint32_t id; /* XML:integer */
    uint32_t instance_count; /* Should be >= 2 */
    uint32_t metadata_count;
};

struct gmio_amf_instance
{
    uint32_t objectid; /* XML:nonNegativeInteger */
    struct gmio_vec3d delta; /* Any value */
    struct gmio_vec3d rot; /* XML:Degrees: -360 <= coord < 360 */
};

enum gmio_amf_texture_type
{
    GMIO_AMF_TEXTURE_TYPE_GRAYSCALE
};

struct gmio_amf_texture
{
    uint32_t id; /* XML:integer */
    uint32_t width;  /* XML:nonNegativeInteger */
    uint32_t height; /* XML:nonNegativeInteger */
    uint32_t depth;  /* XML:nonNegativeInteger */
    bool tiled;
    enum gmio_amf_texture_type type;
    struct gmio_memblock binary_data; /* Will be converted to base64 */
};

enum gmio_amf_unit
{
    GMIO_AMF_UNIT_UNKNOWN,
    GMIO_AMF_UNIT_MILLIMETER,
    GMIO_AMF_UNIT_INCH,
    GMIO_AMF_UNIT_FEET,
    GMIO_AMF_UNIT_METER,
    GMIO_AMF_UNIT_MICRON
};

enum gmio_amf_document_element
{
    GMIO_AMF_DOCUMENT_ELEMENT_OBJECT,
    GMIO_AMF_DOCUMENT_ELEMENT_MATERIAL,
    GMIO_AMF_DOCUMENT_ELEMENT_TEXTURE,
    GMIO_AMF_DOCUMENT_ELEMENT_CONSTELLATION,
    GMIO_AMF_DOCUMENT_ELEMENT_METADATA
};

enum gmio_amf_mesh_element
{
    GMIO_AMF_MESH_ELEMENT_VERTEX,
    GMIO_AMF_MESH_ELEMENT_EDGE,
    GMIO_AMF_MESH_ELEMENT_VOLUME
};

struct gmio_amf_object_mesh_element_index
{
    uint32_t object_index;
    uint32_t mesh_index;
    uint32_t value;
};

/* Note: version is 1.2 */
struct gmio_amf_document
{
    /*! Opaque pointer on the user AMF document, passed as first argument to
     *  hook functions */
    const void* cookie;

    enum gmio_amf_unit unit;
    uint32_t object_count; /* Must be >= 1 */
    uint32_t material_count;
    uint32_t texture_count;
    uint32_t constellation_count;
    uint32_t metadata_count;

    /*! Pointer on a function that retrieves the i-th document sub-element
     *
     *  \p element is the type of the sub-element of interest.\n
     *  \p element_index is the index of the sub-element within the AMF
     *  document.\n
     *  The domain of this index depends on \p element :
     *  <table>
     *    <tr> <th>Element type</th>  <th>Domain of index</th> </tr>
     *    <tr> <td>OBJECT</td>   <td><tt>[0 .. object_count[</tt></td>
     *    <tr> <td>MATERIAL</td> <td><tt>[0 .. material_count[</tt></td>
     *    <tr> <td>TEXTURE</td>  <td><tt>[0 .. texture_count[</tt></td>
     *    <tr> <td>CONSTELLATION</td>  <td><tt>[0 .. constellation_count[</tt></td>
     *    <tr> <td>METADATA</td>  <td><tt>[0 .. metadata_count[</tt></td>
     *  </table>
     *
     *  <table>
     *    <tr> <th>Element type</th>  <th>gmio type</th> </tr>
     *    <tr> <td>OBJECT</td>   <td>gmio_amf_object</td>
     *    <tr> <td>MATERIAL</td> <td>gmio_amf_material</td>
     *    <tr> <td>TEXTURE</td>  <td>gmio_amf_texture</td>
     *    <tr> <td>CONSTELLATION</td>  <td>gmio_amf_constellation</td>
     *    <tr> <td>METADATA</td> <td>gmio_amf_metadata</td>
     *  </table>
     */
    void (*func_get_document_element)(
            const void* cookie,
            enum gmio_amf_document_element element,
            uint32_t element_index,
            void* ptr_element);

    /*! Pointer on a function that retrieves the i-th \c composite within a
     *  \c material element */
    void (*func_get_material_composite)(
            const void* cookie,
            uint32_t material_index,
            uint32_t composite_index,
            struct gmio_amf_composite* ptr_composite);

    /*! Pointer on a function that retrieves the i-th \c instance within a
     *  \c constellation element */
    void (*func_get_constellation_instance)(
            const void* cookie,
            uint32_t constellation_index,
            uint32_t instance_index,
            struct gmio_amf_instance* ptr_instance);

    /*! Pointer on a function that retrieves the i-th \c mesh within an
     *  \c object element */
    void (*func_get_object_mesh)(
            const void* cookie,
            uint32_t object_index,
            uint32_t mesh_index,
            struct gmio_amf_mesh* ptr_mesh);

    /*! Pointer on a function that retrieves the i-th sub-element of a \c mesh
     *  element
     *
     *  \p element is the type of the sub-element of interest.\n
     *  \p element_index is the index of the sub-element within the \c mesh
     *  element.\n
     *  The domain of this index(ie. \c value field) depends on \p element :
     *  <table>
     *    <tr> <th>Element type</th>  <th>Domain of index</th> </tr>
     *    <tr> <td>VERTEX</td> <td><tt>[0 .. gmio_amf_mesh::vertex_count[</tt></td>
     *    <tr> <td>EDGE</td>   <td><tt>[0 .. gmio_amf_mesh::edge_count[</tt></td>
     *    <tr> <td>VOLUME</td> <td><tt>[0 .. gmio_amf_mesh::volume_count[</tt></td>
     *  </table>
     *
     *  The concrete type of \p ptr_struct_element depends on \p element :
     *  <table>
     *    <tr> <th>Element type</th>  <th>gmio type</th> </tr>
     *    <tr> <td>VERTEX</td> <td>gmio_amf_vertex</td>
     *    <tr> <td>EDGE</td>   <td>gmio_amf_edge</td>
     *    <tr> <td>VOLUME</td> <td>gmio_amf_volume</td>
     *  </table>
     */
    void (*func_get_object_mesh_element)(
            const void* cookie,
            enum gmio_amf_mesh_element element,
            const struct gmio_amf_object_mesh_element_index* element_index,
            void* ptr_element);

    /*! Pointer on a function that retrieves the i-th \c triangle within a
     *  mesh \c volume element */
    void (*func_get_object_mesh_volume_triangle)(
            const void* cookie,
            const struct gmio_amf_object_mesh_element_index* volume_index,
            uint32_t triangle_index,
            struct gmio_amf_triangle* ptr_triangle);

    /* Function pointers to retrieve metadata */

    /*! Pointer on a function that retrieves the i-th metadata assigned to a
     *  document sub-element(being \c material, \c object or \c constellation )
     *
     *  \p element is the type of the sub-element of interest, it can
     *  take one of this value :
     *      \li GMIO_AMF_DOCUMENT_ELEMENT_MATERIAL
     *      \li GMIO_AMF_DOCUMENT_ELEMENT_OBJECT
     *      \li GMIO_AMF_DOCUMENT_ELEMENT_CONSTELLATION
     *
     *  \p element_index is the index of the sub-element within the AMF
     *  document.\n
     *  The domain of this index depends on \p element :
     *  <table>
     *    <tr> <th>Element type</th>  <th>Domain of index</th> </tr>
     *    <tr> <td>MATERIAL</td> <td><tt>[0 .. material_count[</tt></td>
     *    <tr> <td>OBJECT</td>   <td><tt>[0 .. object_count[</tt></td>
     *    <tr> <td>CONSTELLATION</td> <td><tt>[0 .. constellation_count[</tt></td>
     *  </table>
     */
    void (*func_get_document_element_metadata)(
            const void* cookie,
            enum gmio_amf_document_element element,
            uint32_t element_index,
            uint32_t metadata_index,
            struct gmio_amf_metadata* ptr_metadata);

    /*! Pointer on a function that retrieves the i-th metadata assigned to a
     *  mesh vertex */
    void (*func_get_object_mesh_vertex_metadata)(
            const void* cookie,
            const struct gmio_amf_object_mesh_element_index* vertex_index,
            uint32_t metadata_index,
            struct gmio_amf_metadata* ptr_metadata);

    /*! Pointer on a function that retrieves the i-th metadata assigned to a
     *  mesh volume */
    void (*func_get_object_mesh_volume_metadata)(
            const void* cookie,
            const struct gmio_amf_object_mesh_element_index* volume_index,
            uint32_t metadata_index,
            struct gmio_amf_metadata* ptr_metadata);
};

#endif /* GMIO_AMF_DOCUMENT_H */
