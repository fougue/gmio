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

#include <Standard_Version.hxx>
#if OCC_VERSION_HEX >= 0x070200
#  error OpenCascade STL_Mesh was removed from OpenCascade >= v7.2.0
#endif

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

namespace gmio {

//! Provides access to all the triangles of an OpenCascade \c StlMesh_Mesh
//!
//!  gmio_stl_mesh_occmesh iterates efficiently over the triangles of all
//!  domains.
//!
//!  Example of use:
//!  \code{.cpp}
//!      const Handle_StlMesh_Mesh meshocc = ...;
//!      gmio::STL_write(stl_format, filepath, gmio::STL_MeshOcc(mesh));
//!  \endcode
class STL_MeshOcc : public STL_Mesh {
public:
    STL_MeshOcc();
    explicit STL_MeshOcc(const Handle_StlMesh_Mesh& hnd);

    const Handle_StlMesh_Mesh& mesh() const { return m_mesh; }

    STL_Triangle triangle(uint32_t tri_id) const override;

private:
    void init();

    struct DomainData {
        std::vector<const gp_XYZ*> vec_coords;
    };

    struct TriangleData {
        const StlMesh_MeshTriangle* ptr_triangle;
        const DomainData* ptr_domain;
    };

    Handle_StlMesh_Mesh m_mesh;
    std::vector<DomainData> m_vec_domain_data;
    std::vector<TriangleData> m_vec_triangle_data;
};

//!/ Provides creation of a new domain within an OpenCascade \c StlMesh_Mesh
//!
//!  STL_MeshCreatorOcc::addTriangle() calls
//!  <tt>StlMesh_Mesh::AddVertex()</tt> only for new unique vertices, ie. they
//!  are no vertex duplicates in the resulting domain.
//!
//!  As of OpenCascade v7.0.0, it's not possible to rely on
//!  <tt>StlMesh_Mesh::AddOnlyNewVertex()</tt>: this function
//!  still has the same effect as <tt>StlMesh_Mesh::AddVertex()</tt>
//!
//!  Example of use:
//!  \code{.cpp}
//!      Handle_StlMesh_Mesh mesh = new StlMesh_Mesh;
//!      gmio::STL_MeshCreatorOcc meshcreator(mesh);
//!      gmio::STL_read(filepath, &meshcreator);
//!  \endcode
class STL_MeshCreatorOcc : public STL_MeshCreator {
public:
    STL_MeshCreatorOcc();
    explicit STL_MeshCreatorOcc(const Handle_StlMesh_Mesh& hnd);

    const Handle_StlMesh_Mesh& mesh() const { return m_mesh; }

    void beginSolid(const STL_MeshCreatorInfos& infos) override;
    void addTriangle(uint32_t tri_id, const STL_Triangle& triangle) override;
    void endSolid() override;

private:
    int addUniqueVertex(const Vec3f& v);

    Handle_StlMesh_Mesh m_mesh;
    BRepBuilderAPI_CellFilter m_filter;
    BRepBuilderAPI_VertexInspector m_inspector;
};

} // namespace gmio

/*! @} */
