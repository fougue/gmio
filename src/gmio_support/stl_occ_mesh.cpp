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

// -----------------------------------------------------------------------------
// gmio_stl_mesh_occmesh
// -----------------------------------------------------------------------------

gmio_stl_mesh_occmesh::gmio_stl_mesh_occmesh()
    : m_mesh(NULL)
{
    this->init_C_members();
}

gmio_stl_mesh_occmesh::gmio_stl_mesh_occmesh(const StlMesh_Mesh *mesh)
    : m_mesh(mesh)
{
    this->init_C_members();
    this->init_cache();
}

gmio_stl_mesh_occmesh::gmio_stl_mesh_occmesh(const Handle_StlMesh_Mesh &hnd)
    : m_mesh(hnd.operator->())
{
    this->init_C_members();
    this->init_cache();
}

void gmio_stl_mesh_occmesh::init_C_members()
{
    this->cookie = this;
    this->func_get_triangle = &gmio_stl_mesh_occmesh::get_triangle;
    this->triangle_count = 0;
}

void gmio_stl_mesh_occmesh::get_triangle(
        const void *cookie, uint32_t tri_id, gmio_stl_triangle *tri)
{
    const gmio_stl_mesh_occmesh* it =
            static_cast<const gmio_stl_mesh_occmesh*>(cookie);
    const triangle_data& tridata = it->m_vec_triangle_data.at(tri_id);
    const std::vector<const gp_XYZ*>& vec_coords = tridata.ptr_domain->vec_coords;

    int iv1, iv2, iv3;
    double nx, ny, nz;
    tridata.ptr_triangle->GetVertexAndOrientation(iv1, iv2, iv3, nx, ny, nz);
    gmio_stl_occ_copy_xyz(&tri->n, nx, ny, nz);
    gmio_stl_occ_copy_xyz(&tri->v1, *vec_coords.at(iv1 - 1));
    gmio_stl_occ_copy_xyz(&tri->v2, *vec_coords.at(iv2 - 1));
    gmio_stl_occ_copy_xyz(&tri->v3, *vec_coords.at(iv3 - 1));
}

void gmio_stl_mesh_occmesh::init_cache()
{
    // Count triangles
    const int domain_count = m_mesh != NULL ? m_mesh->NbDomains() : 0;
    for (int dom_id = 1; dom_id <= domain_count; ++dom_id)
        this->triangle_count += m_mesh->NbTriangles(dom_id);

    // Fill vector of triangle data
    m_vec_domain_data.resize(domain_count);
    m_vec_triangle_data.resize(this->triangle_count);
    std::size_t vec_tri_id = 0;
    for (int dom_id = 1; dom_id <= domain_count; ++dom_id) {
        // Cache vertex indexes
        //   TColgp_SequenceOfXYZ::Value(int) is slow(linear search)
        const TColgp_SequenceOfXYZ& seq_vertices = m_mesh->Vertices(dom_id);
        struct domain_data& domdata = m_vec_domain_data.at(dom_id - 1);
        domdata.vec_coords.reserve(seq_vertices.Length());
#if OCC_VERSION_HEX >= 0x070000
        typedef TColgp_SequenceOfXYZ::const_iterator ConstIterSeqXYZ;
        const ConstIterSeqXYZ seq_end = seq_vertices.cend();
        for (ConstIterSeqXYZ it = seq_vertices.cbegin(); it != seq_end; ++it)
            domdata.vec_coords.push_back(&(*it));
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
            struct triangle_data& tridata = m_vec_triangle_data.at(vec_tri_id);
            tridata.ptr_triangle = hnd_occtri.operator->();
            tridata.ptr_domain = &domdata;
            ++vec_tri_id;
        }
    }
}

// -----------------------------------------------------------------------------
// gmio_stl_mesh_creator_occmesh
// -----------------------------------------------------------------------------

gmio_stl_mesh_creator_occmesh::gmio_stl_mesh_creator_occmesh()
    : m_mesh(NULL),
      m_filter(Precision::Confusion()),
      m_inspector(Precision::Confusion())
{
    this->init_C_members();
}

gmio_stl_mesh_creator_occmesh::gmio_stl_mesh_creator_occmesh(StlMesh_Mesh *mesh)
    : m_mesh(mesh),
      m_filter(Precision::Confusion()),
      m_inspector(Precision::Confusion())
{
    this->init_C_members();
}

gmio_stl_mesh_creator_occmesh::gmio_stl_mesh_creator_occmesh(
        const Handle_StlMesh_Mesh &hnd)
    : m_mesh(hnd.operator->()),
      m_filter(Precision::Confusion()),
      m_inspector(Precision::Confusion())
{
    this->init_C_members();
}

void gmio_stl_mesh_creator_occmesh::begin_solid(
        void* cookie, const gmio_stl_mesh_creator_infos* /*infos*/)
{
    gmio_stl_mesh_creator_occmesh* creator =
            static_cast<gmio_stl_mesh_creator_occmesh*>(cookie);
    creator->m_mesh->AddDomain();
}

void gmio_stl_mesh_creator_occmesh::add_triangle(
        void *cookie, uint32_t /*tri_id*/, const gmio_stl_triangle *tri)
{
    gmio_stl_mesh_creator_occmesh* creator =
            static_cast<gmio_stl_mesh_creator_occmesh*>(cookie);
    const gmio_vec3f& n = tri->n;
    creator->m_mesh->AddTriangle(
                creator->add_unique_vertex(tri->v1),
                creator->add_unique_vertex(tri->v2),
                creator->add_unique_vertex(tri->v3),
                n.x, n.y, n.z);

}

void gmio_stl_mesh_creator_occmesh::init_C_members()
{
    this->cookie = this;
    this->func_begin_solid = &gmio_stl_mesh_creator_occmesh::begin_solid;
    this->func_add_triangle = &gmio_stl_mesh_creator_occmesh::add_triangle;
    this->func_end_solid = NULL;
}

int gmio_stl_mesh_creator_occmesh::add_unique_vertex(const gmio_vec3f& v)
{
    //--------------------------------------------------------------------------
    // Code excerpted from OpenCascade v7.0.0
    //     File: RWStl/RWStl.cxx
    //     Function: "static int AddVertex(...)" lines 38..61
    //--------------------------------------------------------------------------
    const gp_XYZ pnt(v.x, v.y, v.z);
    m_inspector.SetCurrent(pnt);
    const gp_XYZ min_pnt = m_inspector.Shift(pnt, -Precision::Confusion());
    const gp_XYZ max_pnt = m_inspector.Shift(pnt, +Precision::Confusion());
    m_filter.Inspect(min_pnt, max_pnt, m_inspector);
    if (!m_inspector.ResInd().IsEmpty()) {
      const int index = m_inspector.ResInd().First(); // There should be only one
      m_inspector.ClearResList();
      return index;
    }
    const int index = m_mesh->AddVertex(pnt.X(), pnt.Y(), pnt.Z());
    m_filter.Add(index, pnt);
    m_inspector.Add(pnt);
    return index;
}
