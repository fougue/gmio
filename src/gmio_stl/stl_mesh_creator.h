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

/*! \file stl_mesh_creator.h
 *  Declaration of gmio_stl_mesh_creator
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#pragma once

#include "stl_global.h"
#include "stl_format.h"
#include "stl_triangle.h"
#include "stlb_header.h"
#include "../gmio_core/stream.h"

#include <stddef.h>

/*! Informations about the STL stream, used in
 *  gmio_stl_mesh_creator::func_begin_solid() */
struct gmio_stl_mesh_creator_infos
{
    /*! Format of the input STL mesh */
    enum gmio_stl_format format;

    /*! Null terminated C-string holding the STL mesh(solid) name.
     *  Available only if STL ASCII format, \c NULL otherwise */
    const char* stla_solid_name;

    /*! Total size (in bytes) of the input stream
     *
     *  This is the result of gmio_stl_read_options::func_stla_get_streamsize()
     *
     *  Useful to roughly estimate the facet count in the input mesh.
     *  Available only if STL ASCII format, \c 0 otherwise
     */
    gmio_streamsize_t stla_stream_size;

    /*! Contains the header data(80 bytes).
     *  Available only if binary STL, \c NULL otherwise */
    const struct gmio_stlb_header* stlb_header;

    /*! Count of mesh facets(triangles).
     *  Available only if binary STL, \c 0 otherwise */
    uint32_t stlb_triangle_count;
};

/*! Provides an interface for the creation of the underlying(hidden)
 *  user mesh */
struct gmio_stl_mesh_creator
{
    /*! Opaque pointer on the user mesh, passed as first argument to hook
     *  functions */
    void* cookie;

    /* All function pointers are optional (ie can be set to NULL) */

    /*! Optional function that handles declaration of a solid */
    void (*func_begin_solid)(
            void* cookie, const struct gmio_stl_mesh_creator_infos* infos);

    /*! Function that adds a triangle to the user mesh
     *
     *  The argument \p triangle is the triangle to be added, note that
     *  struct gmio_stl_triangle::attribute_byte_count is meaningless for STL
     *  ascii.
     *
     *  The argument \p tri_id is the index of the mesh triangle
     */
    void (*func_add_triangle)(
            void* cookie,
            uint32_t tri_id,
            const struct gmio_stl_triangle* triangle);

    /*! Optional function that finalizes creation of the user mesh
     *
     *  The function is called at the end of the read process, ie. after all
     *  triangles have been added */
    void (*func_end_solid)(void* cookie);
};

/*! @} */
