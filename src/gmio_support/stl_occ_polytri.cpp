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

#include <gmio_support/stl_occ_polytri.h>

#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <cstring>

namespace gmio {

// -----------------------------------------------------------------------------
// STL_MeshOccPolyTriangulation
// -----------------------------------------------------------------------------

STL_MeshOccPolyTriangulation::STL_MeshOccPolyTriangulation()
{
    this->init();
}

STL_MeshOccPolyTriangulation::STL_MeshOccPolyTriangulation(
        const Handle_Poly_Triangulation& hnd)
    : m_polytri(hnd)
{
    this->init();
}

STL_Triangle STL_MeshOccPolyTriangulation::triangle(uint32_t tri_id) const
{
    STL_Triangle tri;
    tri.attribute_byte_count = 0;

    const TColgp_Array1OfPnt& vec_node = *m_polytri_vec_node;
    const Poly_Array1OfTriangle& vec_triangle = *m_polytri_vec_triangle;
    int n1, n2, n3; // Node index
    vec_triangle.Value(tri_id + vec_triangle.Lower()).Get(n1, n2, n3);
    const gp_Pnt& p1 = vec_node.Value(n1);
    const gp_Pnt& p2 = vec_node.Value(n2);
    const gp_Pnt& p3 = vec_node.Value(n3);
    OCC_copyCoords(&tri.v1, p1.XYZ());
    OCC_copyCoords(&tri.v2, p2.XYZ());
    OCC_copyCoords(&tri.v3, p3.XYZ());
    if (m_polytri_has_normals) {
        const TShort_Array1OfShortReal& vec_normal = *m_polytri_vec_normal;
        // Take the normal at the first triangle node
        std::memcpy(&tri.n, &vec_normal.Value(n1*3), 3*sizeof(float));
    }
    else {
        tri.n = STL_triangleNormal(tri);
    }

    return tri;
}

void STL_MeshOccPolyTriangulation::init()
{
    const bool polytri_not_null = !m_polytri.IsNull();
    this->setTriangleCount(polytri_not_null ? m_polytri->NbTriangles() : 0);
    m_polytri_vec_node = polytri_not_null ? &m_polytri->Nodes() : nullptr;
    m_polytri_vec_triangle = polytri_not_null ? &m_polytri->Triangles() : nullptr;
    m_polytri_has_normals =
            polytri_not_null ?
                (m_polytri->HasNormals() == Standard_True) :
                false;
    m_polytri_vec_normal = m_polytri_has_normals ? &m_polytri->Normals() : nullptr;
}

// -----------------------------------------------------------------------------
// STL_MeshCreatorOccPolyTriangulation
// -----------------------------------------------------------------------------

STL_MeshCreatorOccPolyTriangulation::STL_MeshCreatorOccPolyTriangulation()
{
}

void STL_MeshCreatorOccPolyTriangulation::beginSolid(const STL_MeshCreatorInfos &infos)
{
    uint32_t tricount = 0;
    if (infos.format == STL_Format_Ascii)
        tricount = static_cast<uint32_t>(infos.ascii_solid_size / 160u);
    else if (infos.format & STL_Format_TagBinary)
        tricount = infos.binary_triangle_count;
    if (tricount > 0) {
        m_vec_node.reserve(3 * tricount);
        m_vec_normal.reserve(3 * tricount);
        m_vec_triangle.reserve(tricount);
    }
}

void STL_MeshCreatorOccPolyTriangulation::addTriangle(
        uint32_t, const STL_Triangle &triangle)
{
    const int id_v1 = this->addUniqueNode(OCC_fromVec3(triangle.v1), triangle.n);
    const int id_v2 = this->addUniqueNode(OCC_fromVec3(triangle.v2), triangle.n);
    const int id_v3 = this->addUniqueNode(OCC_fromVec3(triangle.v3), triangle.n);
    m_vec_triangle.emplace_back(id_v1, id_v2, id_v3);
}

void STL_MeshCreatorOccPolyTriangulation::endSolid()
{
    m_polytri =
            new Poly_Triangulation(
                static_cast<int>(m_vec_node.size()),
                static_cast<int>(m_vec_triangle.size()),
                Standard_False); // False->No UVNodes
    // Copy nodes
    TColgp_Array1OfPnt* nodes = &(m_polytri->ChangeNodes());
    std::memcpy(&(nodes->ChangeValue(nodes->Lower())),
                m_vec_node.data(),
                m_vec_node.size() * sizeof(gp_XYZ));
    // Copy normals
    Handle_TShort_HArray1OfShortReal normals =
            new TShort_HArray1OfShortReal(1, static_cast<int>(3 * m_vec_normal.size()));
    std::memcpy(&normals->ChangeValue(normals->Lower()),
                m_vec_normal.data(),
                m_vec_normal.size() * sizeof(Vec3f));
    m_polytri->SetNormals(normals);
    // Copy triangles
    Poly_Array1OfTriangle* triangles = &(m_polytri->ChangeTriangles());
    std::memcpy(&triangles->ChangeValue(triangles->Lower()),
                m_vec_triangle.data(),
                m_vec_triangle.size() * sizeof(Poly_Triangle));
}

int gmio::STL_MeshCreatorOccPolyTriangulation::addUniqueNode(
        const gp_XYZ &coords, const Vec3f& normal)
{
    int index = m_merge_tool.findIndex(coords);
    if (index == -1) {
        index = m_merge_tool.addNode(coords);
        m_vec_node.push_back(std::move(coords));
        m_vec_normal.push_back(normal);
    }
    return index;
}

} // namespace gmio
