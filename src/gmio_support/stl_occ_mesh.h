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

/*! \file stl_occ_mesh.h
 *  STL support of OpenCascade's \c StlMesh_Mesh
 *
 *  To use this header the source file
 *      <tt>$INSTALL/src/gmio_support/stl_occ_mesh.cpp</tt>\n
 *  needs to be built in the target project(\c $INSTALL is the root directory
 *  where is installed gmio)
 *
 *  Of course this requires the includepath and libpath to point to OpenCascade,
 *  the import libraries likely needed are:\n
 *      <tt>TKernel TKMath TKSTL TKTopAlgo</tt>
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
#include <StlMesh_Mesh.hxx>
#include <StlMesh_MeshTriangle.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <vector>

/*! Provides access to all the triangles of an OpenCascade \c StlMesh_Mesh
 *
 *  gmio_stl_mesh_occmesh iterates efficiently over the triangles of all
 *  domains.
  *
 *  Example of use:
 *  \code{.cpp}
 *      const Handle_StlMesh_Mesh occmesh = ...;
 *      const gmio_stl_mesh_occmesh mesh(occmesh);
 *      gmio_stl_write_file(stl_format, filepath, &mesh, &options);
 *  \endcode
 */
struct gmio_stl_mesh_occmesh : public gmio_stl_mesh
{
    gmio_stl_mesh_occmesh();
    explicit gmio_stl_mesh_occmesh(const Handle_StlMesh_Mesh& hnd);

    const Handle_StlMesh_Mesh& mesh() const { return m_mesh; }

private:
    static void get_triangle(
            const void* cookie, uint32_t tri_id, gmio_stl_triangle* tri);

    void init();

    struct domain_data {
        std::vector<const gp_XYZ*> vec_coords;
    };

    struct triangle_data {
        const StlMesh_MeshTriangle* ptr_triangle;
        const domain_data* ptr_domain;
    };

    Handle_StlMesh_Mesh m_mesh;
    std::vector<domain_data> m_vec_domain_data;
    std::vector<triangle_data> m_vec_triangle_data;
};

/*! Provides creation of a new domain within an OpenCascade \c StlMesh_Mesh
 *
 *  gmio_stl_mesh_creator::func_add_triangle() calls
 *  <tt>StlMesh_Mesh::AddVertex()</tt> only for new unique vertices, ie. they
 *  are no vertex duplicates in the resulting domain.
 *
 *  As of OpenCascade v7.0.0, it's not possible to rely on
 *  <tt>StlMesh_Mesh::AddOnlyNewVertex()</tt>: this function
 *  still has the same effect as <tt>StlMesh_Mesh::AddVertex()</tt>
 *
 *  Example of use:
 *  \code{.cpp}
 *      Handle_StlMesh_Mesh occmesh = new StlMesh_Mesh;
 *      gmio_stl_mesh_creator_occmesh meshcreator(occmesh);
 *      gmio_stl_read_file(filepath, &meshcreator, &options);
 *  \endcode
 */
struct gmio_stl_mesh_creator_occmesh : public gmio_stl_mesh_creator
{
    gmio_stl_mesh_creator_occmesh();
    explicit gmio_stl_mesh_creator_occmesh(const Handle_StlMesh_Mesh& hnd);

    const Handle_StlMesh_Mesh& mesh() const { return m_mesh; }

private:
    static void begin_solid(
            void* cookie, const struct gmio_stl_mesh_creator_infos* infos);
    static void add_triangle(
            void* cookie, uint32_t tri_id, const gmio_stl_triangle* tri);

    void init();
    int add_unique_vertex(const gmio_vec3f& v);

    Handle_StlMesh_Mesh m_mesh;
    BRepBuilderAPI_CellFilter m_filter;
    BRepBuilderAPI_VertexInspector m_inspector;
};

/*! @} */
