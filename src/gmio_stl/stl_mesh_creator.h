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

#include <string>

namespace gmio {

struct STL_MeshCreatorInfos;

//! Provides an interface for the creation of the underlying(hidden) user mesh
class GMIO_API STL_MeshCreator {
public:
    //! Handles declaration of a solid (does nothing by default)
    virtual void beginSolid(const STL_MeshCreatorInfos& infos);

    //! Adds a triangle to the user mesh
    //!
    //! The argument 'triangle' is the triangle to be added, note that
    //! struct STL_Triangle::attribute_byte_count is meaningless for STL ascii.
    //!
    //! The argument 'tri_id' is the index of the mesh triangle
    virtual void addTriangle(uint32_t tri_id, const STL_Triangle& triangle);

    //! Finalizes creation of the user mesh
    //!
    //! The function is called at the end of the read process, ie. after all
    //! triangles have been added
    virtual void endSolid();
};

//! Informations about the STL stream, used in STL_MeshCreator::beginSolid()
struct STL_MeshCreatorInfos {
    //! Format of the input STL mesh
    STL_Format format;

    //! STL mesh(solid) name.
    //! Available only if STL ASCII format
    std::string ascii_solid_name;

    //! Total size (in bytes) of the input stream
    //!
    //! Useful to roughly estimate the facet count in the input mesh.
    //! Available only if STL ASCII format, \c 0 otherwise
    uint64_t ascii_solid_size;

    //! Contains the header data(80 bytes).
    //! Available only if binary STL
    STL_BinaryHeader binary_header;

    //! Count of mesh facets(triangles).
    //! Available only if binary STL, \c 0 otherwise
    uint32_t binary_triangle_count;
};

} // namespace gmio

/*! @} */
