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

/*! \file stl_occ_polytri.h
 *  STL support of OpenCascade's \c Poly_Triangulation
 *
 *  To use this header the source file
 *      <tt>$INSTALL/src/gmio_support/stl_occ_polytri.cpp</tt>\n
 *  needs to be built in the target project(\c $INSTALL is the root directory
 *  where is installed gmio)
 *
 *  Of course this requires the includepath and libpath to point to OpenCascade,
 *  the import libraries likely needed are:\n
 *      <tt>TKernel TKMath TKTopAlgo</tt>
 *
 *  \addtogroup gmio_support
 *  @{
 */

#ifndef __cplusplus
#  error C++ compiler required
#endif

#pragma once

#include "support_global.h"
#include "../gmio_stl/stl_mesh.h"
#include "../gmio_stl/stl_mesh_creator.h"

#include <BRepBuilderAPI_CellFilter.hxx>
#include <BRepBuilderAPI_VertexInspector.hxx>
#include <Poly_Triangulation.hxx>
#include <gp_XYZ.hxx>
#include <vector>

/*! Provides access to the triangles of an OpenCascade \c Poly_Triangulation
 *
 *  Example of use:
 *  \code{.cpp}
 *      const Handle_Poly_Triangulation occpolytri = ...;
 *      const gmio_stl_mesh_occpolytri mesh(occpolytri);
 *      gmio_stl_write_file(stl_format, filepath, &occmesh, &options);
 *  \endcode
 */
struct gmio_stl_mesh_occpolytri : public gmio_stl_mesh
{
    gmio_stl_mesh_occpolytri();
    explicit gmio_stl_mesh_occpolytri(const Handle_Poly_Triangulation& hnd);

    const Handle_Poly_Triangulation& polytri() const { return m_polytri; }

private:
    static void get_triangle(
            const void* cookie, uint32_t tri_id, gmio_stl_triangle* tri);
    void init();

    Handle_Poly_Triangulation m_polytri;
    const TColgp_Array1OfPnt* m_polytri_vec_node;
    const Poly_Array1OfTriangle* m_polytri_vec_triangle;
    bool m_polytri_has_normals;
    const TShort_Array1OfShortReal* m_polytri_vec_normal;
};

/*! Provides creation of an OpenCascade \c Poly_Triangulation
 *
 *  Example of use:
 *  \code{.cpp}
 *      gmio_stl_mesh_creator_occpolytri meshcreator;
 *      gmio_stl_read_file(filepath, &meshcreator, &options);
 *      Handle_Poly_Triangulation occpolytri = meshcreator.polytri();
 *  \endcode
 */
struct gmio_stl_mesh_creator_occpolytri : public gmio_stl_mesh_creator
{
public:
    gmio_stl_mesh_creator_occpolytri();
    const Handle_Poly_Triangulation& polytri() const { return m_polytri; }

private:
    static void begin_solid(
            void* cookie, const struct gmio_stl_mesh_creator_infos* infos);
    static void add_triangle(
            void* cookie, uint32_t tri_id, const gmio_stl_triangle* tri);
    static void end_solid(void* cookie);

    int add_unique_vertex(const gmio_vec3f& v, const gmio_vec3f& n);

    Handle_Poly_Triangulation m_polytri;
    BRepBuilderAPI_CellFilter m_filter;
    BRepBuilderAPI_VertexInspector m_inspector;
    std::vector<gp_XYZ> m_vec_node;
    std::vector<gmio_vec3f> m_vec_normal;
    std::vector<Poly_Triangle> m_vec_triangle;
};

/*! @} */
