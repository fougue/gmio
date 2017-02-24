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
#include "stl_occ_utils.h"

#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <cstring>

// -----------------------------------------------------------------------------
// gmio_stl_mesh_occpolytri
// -----------------------------------------------------------------------------

gmio_stl_mesh_occpolytri::gmio_stl_mesh_occpolytri()
{
    this->init();
}

gmio_stl_mesh_occpolytri::gmio_stl_mesh_occpolytri(
        const Handle_Poly_Triangulation& hnd)
    : m_polytri(hnd)
{
    this->init();
}

void gmio_stl_mesh_occpolytri::init()
{
    const bool polytri_not_null = !m_polytri.IsNull();
    // C members
    this->cookie = this;
    this->func_get_triangle = &gmio_stl_mesh_occpolytri::get_triangle;
    this->triangle_count = polytri_not_null ? m_polytri->NbTriangles() : 0;
    // Cache
    m_polytri_vec_node = polytri_not_null ? &m_polytri->Nodes() : NULL;
    m_polytri_vec_triangle = polytri_not_null ? &m_polytri->Triangles() : NULL;
    m_polytri_has_normals =
            polytri_not_null ?
                (m_polytri->HasNormals() == Standard_True) :
                false;
    m_polytri_vec_normal = m_polytri_has_normals ? &m_polytri->Normals() : NULL;
}

void gmio_stl_mesh_occpolytri::get_triangle(
        const void *cookie, uint32_t tri_id, gmio_stl_triangle *tri)
{
    const gmio_stl_mesh_occpolytri* mesh =
            static_cast<const gmio_stl_mesh_occpolytri*>(cookie);
    const TColgp_Array1OfPnt& vec_node = *mesh->m_polytri_vec_node;
    const Poly_Array1OfTriangle& vec_triangle = *mesh->m_polytri_vec_triangle;
    int n1, n2, n3; // Node index
    vec_triangle.Value(tri_id + vec_triangle.Lower()).Get(n1, n2, n3);
    const gp_Pnt& p1 = vec_node.Value(n1);
    const gp_Pnt& p2 = vec_node.Value(n2);
    const gp_Pnt& p3 = vec_node.Value(n3);
    gmio_stl_occ_copy_xyz(&tri->v1, p1.XYZ());
    gmio_stl_occ_copy_xyz(&tri->v2, p2.XYZ());
    gmio_stl_occ_copy_xyz(&tri->v3, p3.XYZ());
    if (mesh->m_polytri_has_normals) {
        const TShort_Array1OfShortReal& vec_normal = *mesh->m_polytri_vec_normal;
        // Take the normal at the first triangle node
        const int id_start_coord = n1*3 + vec_normal.Lower();
        std::memcpy(&tri->n,
                    &vec_normal.Value(id_start_coord),
                    3*sizeof(float));
    }
    else {
        gmio_stl_triangle_compute_normal(tri);
    }
}

// -----------------------------------------------------------------------------
// gmio_stl_mesh_creator_occpolytri
// -----------------------------------------------------------------------------

gmio_stl_mesh_creator_occpolytri::gmio_stl_mesh_creator_occpolytri()
    : m_filter(Precision::Confusion()),
      m_inspector(Precision::Confusion())
{
    this->cookie = this;
    this->func_begin_solid = &gmio_stl_mesh_creator_occpolytri::begin_solid;
    this->func_add_triangle = &gmio_stl_mesh_creator_occpolytri::add_triangle;
    this->func_end_solid = &gmio_stl_mesh_creator_occpolytri::end_solid;
}

void gmio_stl_mesh_creator_occpolytri::begin_solid(
        void* cookie, const gmio_stl_mesh_creator_infos* infos)
{
    gmio_stl_mesh_creator_occpolytri* creator =
            static_cast<gmio_stl_mesh_creator_occpolytri*>(cookie);
    uint32_t tricount = 0;
    if (infos->format == GMIO_STL_FORMAT_ASCII)
        tricount = static_cast<uint32_t>(infos->stla_stream_size / 200u);
    else if (infos->format & GMIO_STL_FORMAT_TAG_BINARY)
        tricount = infos->stlb_triangle_count;
    if (tricount > 0) {
        creator->m_vec_node.reserve(3 * tricount);
        creator->m_vec_normal.reserve(3 * tricount);
        creator->m_vec_triangle.reserve(tricount);
    }
}

void gmio_stl_mesh_creator_occpolytri::add_triangle(
        void *cookie, uint32_t /*tri_id*/, const gmio_stl_triangle *tri)
{
    gmio_stl_mesh_creator_occpolytri* creator =
            static_cast<gmio_stl_mesh_creator_occpolytri*>(cookie);
    const gmio_vec3f& n = tri->n;
    const int id_v1 = creator->add_unique_vertex(tri->v1, n);
    const int id_v2 = creator->add_unique_vertex(tri->v2, n);
    const int id_v3 = creator->add_unique_vertex(tri->v3, n);
    creator->m_vec_triangle.emplace_back(id_v1, id_v2, id_v3);
}

void gmio_stl_mesh_creator_occpolytri::end_solid(void *cookie)
{
    gmio_stl_mesh_creator_occpolytri* creator =
            static_cast<gmio_stl_mesh_creator_occpolytri*>(cookie);
    creator->m_polytri =
            new Poly_Triangulation(
                static_cast<int>(creator->m_vec_node.size()),
                static_cast<int>(creator->m_vec_triangle.size()),
                Standard_False); // False->No UVNodes
    // Copy nodes
    TColgp_Array1OfPnt* nodes = &creator->m_polytri->ChangeNodes();
    std::memcpy(&nodes->ChangeValue(nodes->Lower()),
                creator->m_vec_node.data(),
                creator->m_vec_node.size() * sizeof(gp_XYZ));
    // Copy normals
    Handle_TShort_HArray1OfShortReal normals =
            new TShort_HArray1OfShortReal(
                1, static_cast<int>(3 * creator->m_vec_node.size()));
    std::memcpy(&normals->ChangeValue(normals->Lower()),
                creator->m_vec_normal.data(),
                creator->m_vec_normal.size() * sizeof(gmio_vec3f));
    creator->m_polytri->SetNormals(normals);
    // Copy triangles
    Poly_Array1OfTriangle* triangles = &creator->m_polytri->ChangeTriangles();
    std::memcpy(&triangles->ChangeValue(triangles->Lower()),
                creator->m_vec_triangle.data(),
                creator->m_vec_triangle.size() * sizeof(Poly_Triangle));
}

int gmio_stl_mesh_creator_occpolytri::add_unique_vertex(
        const gmio_vec3f& v, const gmio_vec3f& n)
{
    const gp_XYZ pnt(v.x, v.y, v.z);
    int index = gmio_occ_find_vertex_index(pnt, &m_filter, &m_inspector);
    if (index != -1)
        return index;
    m_vec_node.push_back(pnt);
    m_vec_normal.push_back(n);
    index = static_cast<int>(m_vec_node.size()); // Note: lowerbound = 1
    gmio_occ_add_vertex_index(pnt, index, &m_filter, &m_inspector);
    return index;
}
