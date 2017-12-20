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

/*! \file stl_occ_brep.h
 *  STL support of OpenCascade's \c TopoDS_Shape
 *
 *  To use this header the source files
 *      <tt>$INSTALL/src/gmio_support/stl_occ_brep.cpp</tt>\n
 *      <tt>$INSTALL/src/gmio_support/stl_occ_polytri.cpp</tt>\n
 *  need to be built in the target project(\c $INSTALL is the root directory
 *  where is installed gmio)
 *
 *  Of course this requires the includepath and libpath to point to OpenCascade,
 *  the import libraries likely needed are:\n
 *      <tt>TKernel TKMath TKBRep</tt>
 *
 *  \addtogroup gmio_support
 *  @{
 */

#pragma once

#include "support_global.h"
#include "stl_occ_polytri.h"
#include "../gmio_stl/stl_mesh.h"

#include <Poly_Triangulation.hxx>
#include <TopoDS_Shape.hxx>
#include <vector>

namespace gmio {

//! Provides access to the internal triangles of an OpenCascade \c TopoDS_Shape
//!
//!  gmio_stl_mesh_occshape iterates efficiently over the triangles of all
//!  sub <tt>TopoDS_Faces</tt>(internal \c Poly_Triangulation objects).
//!
//!  Example of use:
//!  \code{.cpp}
//!      const TopoDS_Shape shape = ...;
//!      gmio::STL_write(stl_format, filepath, gmio::STL_MeshOccShape(shape);
//!  \endcode
class STL_MeshOccShape : public STL_Mesh {
public:
    explicit STL_MeshOccShape(const TopoDS_Shape& shape);

    const TopoDS_Shape& shape() const { return m_shape; }

    STL_Triangle triangle(uint32_t tri_id) const override;

private:
    struct FaceData {
        gp_Trsf trsf;
        bool is_reversed;
        const TColgp_Array1OfPnt* ptr_nodes;
    };
    struct TriangleData {
        const Poly_Triangle* ptr_triangle;
        const FaceData* ptr_face_data;
    };

    std::vector<FaceData> m_vec_face_data;
    std::vector<TriangleData> m_vec_triangle_data;
    TopoDS_Shape m_shape;
};

//! Provides creation of an OpenCascade \c TopoDS_Shape containing no
//! geometrical surfaces but only a \c Poly_Triangulation structure
//!
//!  Example of use:
//!  \code{.cpp}
//!      gmio::STL_MeshCreatorOccShape meshcreator;
//!      gmio::STL_read(filepath, &meshcreator);
//!      const TopoDS_Shape shape = meshcreator.shape();
//!  \endcode
class STL_MeshCreatorOccShape : public STL_MeshCreatorOccPolyTriangulation {
public:
    STL_MeshCreatorOccShape();
    TopoDS_Shape& shape() { return m_shape; }

    void endSolid() override;

private:
    TopoDS_Shape m_shape;
};

} // namespace gmio

/*! @} */
