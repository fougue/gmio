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

#ifndef __cplusplus
#  error C++ compiler required
#endif

#pragma once

#include "support_global.h"
#include "stl_occ_polytri.h"
#include "../gmio_stl/stl_mesh.h"

#include <Poly_Triangulation.hxx>
#include <TopoDS_Shape.hxx>
#include <vector>

/*! Provides access to the internal triangles of an OpenCascade \c TopoDS_Shape
 *
 *  gmio_stl_mesh_occshape iterates efficiently over the triangles of all
 *  sub <tt>TopoDS_Faces</tt>(internal \c Poly_Triangulation objects).
 *
 *  Example of use:
 *  \code{.cpp}
 *      const TopoDS_Shape occshape = ...;
 *      const gmio_stl_mesh_occshape mesh(occshape);
 *      gmio_stl_write_file(stl_format, filepath, &occmesh, &options);
 *  \endcode
 */
struct gmio_stl_mesh_occshape : public gmio_stl_mesh
{
    gmio_stl_mesh_occshape();
    explicit gmio_stl_mesh_occshape(const TopoDS_Shape& shape);

    const TopoDS_Shape* shape() const { return m_shape; }

private:
    static void get_triangle(
            const void* cookie, uint32_t tri_id, gmio_stl_triangle* tri);

    void init_C_members();

    struct face_data {
        gp_Trsf trsf;
        bool is_reversed;
        const TColgp_Array1OfPnt* ptr_nodes;
    };
    struct triangle_data {
        const Poly_Triangle* ptr_triangle;
        const face_data* ptr_face_data;
    };

    std::vector<face_data> m_vec_face_data;
    std::vector<triangle_data> m_vec_triangle_data;
    const TopoDS_Shape* m_shape;
};

/*! Provides creation of an OpenCascade \c TopoDS_Shape containing no
 *  geometrical surfaces but only a \c Poly_Triangulation structure
 *
 *  Example of use:
 *  \code{.cpp}
 *      gmio_stl_mesh_creator_occshape meshcreator;
 *      gmio_stl_read_file(filepath, &meshcreator, &options);
 *      const TopoDS_Shape shape = meshcreator.shape();
 *  \endcode
 */
struct gmio_stl_mesh_creator_occshape : public gmio_stl_mesh_creator_occpolytri
{
public:
    gmio_stl_mesh_creator_occshape();
    TopoDS_Shape& shape() { return m_shape; }

private:
    static void end_solid(void* cookie);
    void (*m_func_end_solid_occpolytri)(void* cookie);
    TopoDS_Shape m_shape;
};

/*! @} */
