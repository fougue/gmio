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

#include <gmio_support/stl_occ_mesh.h>

#include "stl_occ_utils.h"

#include <cstring>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlMesh_MeshTriangle.hxx>
#include <StlMesh_SequenceOfMeshTriangle.hxx>
#include <TColgp_SequenceOfXYZ.hxx>

namespace gmio {

// -----------------------------------------------------------------------------
// STL_MeshOcc
// -----------------------------------------------------------------------------

STL_MeshOcc::STL_MeshOcc()
{
    this->init();
}

STL_MeshOcc::STL_MeshOcc(const Handle_StlMesh_Mesh &hnd)
    : m_mesh(hnd)
{
    this->init();
}

STL_Triangle STL_MeshOcc::triangle(uint32_t tri_id) const
{
    const TriangleData& tridata = m_vec_triangle_data.at(tri_id);
    const std::vector<const gp_XYZ*>& vec_coords = tridata.ptr_domain->vec_coords;

    int iv1, iv2, iv3;
    double nx, ny, nz;
    tridata.ptr_triangle->GetVertexAndOrientation(iv1, iv2, iv3, nx, ny, nz);

    STL_Triangle tri;
    tri.attribute_byte_count = 0;
    OCC_copyCoords(&tri.n, nx, ny, nz);
    OCC_copyCoords(&tri.v1, *vec_coords.at(iv1 - 1));
    OCC_copyCoords(&tri.v2, *vec_coords.at(iv2 - 1));
    OCC_copyCoords(&tri.v3, *vec_coords.at(iv3 - 1));
    return tri;
}

void STL_MeshOcc::init()
{
    uint32_t tri_count = 0;
    // Count triangles
    const int domain_count = !m_mesh.IsNull() ? m_mesh->NbDomains() : 0;
    for (int dom_id = 1; dom_id <= domain_count; ++dom_id)
        tri_count += m_mesh->NbTriangles(dom_id);
    this->setTriangleCount(tri_count);
    // Fill vector of triangle data
    m_vec_domain_data.resize(domain_count);
    m_vec_triangle_data.resize(tri_count);
    size_t vec_tri_id = 0;
    for (int dom_id = 1; dom_id <= domain_count; ++dom_id) {
        // Cache vertex indexes
        //   TColgp_SequenceOfXYZ::Value(int) is slow(linear search)
        const TColgp_SequenceOfXYZ& seq_vertices = m_mesh->Vertices(dom_id);
        DomainData& domdata = m_vec_domain_data.at(dom_id - 1);
        domdata.vec_coords.reserve(seq_vertices.Length());
#if OCC_VERSION_HEX >= 0x070000
        for (const gp_XYZ& coords : seq_vertices)
            domdata.vec_coords.push_back(&coords);
#else
        for (int i = 1; i <= seq_vertices.Length(); ++i)
            domdata.vec_coords.push_back(&seq_vertices.Value(i));
#endif

        // Cache triangles
        const StlMesh_SequenceOfMeshTriangle& seq_triangles =
                m_mesh->Triangles(dom_id);
        for (int tri_id = 1; tri_id <= seq_triangles.Length(); ++tri_id) {
            const Handle_StlMesh_MeshTriangle& hnd_occtri =
                    seq_triangles.Value(tri_id);
            TriangleData& tridata = m_vec_triangle_data.at(vec_tri_id);
            tridata.ptr_triangle = hnd_occtri.operator->();
            tridata.ptr_domain = &domdata;
            ++vec_tri_id;
        }
    }
}

// -----------------------------------------------------------------------------
// STL_MeshCreatorOcc
// -----------------------------------------------------------------------------

STL_MeshCreatorOcc::STL_MeshCreatorOcc()
    : m_filter(Precision::Confusion()),
      m_inspector(Precision::Confusion())
{
}

STL_MeshCreatorOcc::STL_MeshCreatorOcc(const Handle_StlMesh_Mesh& hnd)
    : m_mesh(hnd),
      m_filter(Precision::Confusion()),
      m_inspector(Precision::Confusion())
{
}

void STL_MeshCreatorOcc::beginSolid(const STL_MeshCreatorInfos&)
{
    m_mesh->AddDomain();
}

void STL_MeshCreatorOcc::addTriangle(uint32_t, const STL_Triangle& triangle)
{
    const Vec3f& n = triangle.n;
    m_mesh->AddTriangle(
                this->addUniqueVertex(triangle.v1),
                this->addUniqueVertex(triangle.v2),
                this->addUniqueVertex(triangle.v3),
                n.x, n.y, n.z);

}

int STL_MeshCreatorOcc::addUniqueVertex(const Vec3f& v)
{
    const gp_XYZ pnt(v.x, v.y, v.z);
    int index = OCC_findVertexIndex(pnt, &m_filter, &m_inspector);
    if (index != -1)
        return index;
    index = m_mesh->AddVertex(pnt.X(), pnt.Y(), pnt.Z());
    OCC_addVertexIndex(pnt, index, &m_filter, &m_inspector);
    return index;
}

} // namespace gmio
