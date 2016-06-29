/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
** contact@fougue.pro
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
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
    m_vec_triangle_data.reserve(this->triangle_count);
    for (int dom_id = 1; dom_id <= domain_count; ++dom_id) {
        // Cache vertex indexes
        //   TColgp_SequenceOfXYZ::Value(int) is slow(linear search)
        const TColgp_SequenceOfXYZ& seq_vertices = m_mesh->Vertices(dom_id);
        struct domain_data domdata;
        domdata.vec_coords.reserve(seq_vertices.Size());
        typedef TColgp_SequenceOfXYZ::const_iterator ConstIterSeqXYZ;
        const ConstIterSeqXYZ seq_end = seq_vertices.cend();
        for (ConstIterSeqXYZ it = seq_vertices.cbegin(); it != seq_end; ++it)
            domdata.vec_coords.push_back(&(*it));
        m_vec_domain_data.push_back(std::move(domdata));

        // Cache triangles
        const StlMesh_SequenceOfMeshTriangle& seq_triangles =
                m_mesh->Triangles(dom_id);
        for (int tri_id = 1; tri_id <= seq_triangles.Length(); ++tri_id) {
            const Handle_StlMesh_MeshTriangle& hnd_occtri =
                    seq_triangles.Value(tri_id);
            struct triangle_data tridata;
            tridata.ptr_triangle = hnd_occtri.operator->();
            tridata.ptr_domain = &m_vec_domain_data.back();
            m_vec_triangle_data.push_back(std::move(tridata));
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
