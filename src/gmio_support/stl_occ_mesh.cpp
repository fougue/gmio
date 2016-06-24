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
#include <StlMesh_Mesh.hxx>
#include <StlMesh_MeshTriangle.hxx>
#include <StlMesh_SequenceOfMeshTriangle.hxx>
#include <TColgp_SequenceOfXYZ.hxx>

namespace internal {

static void occmesh_add_triangle(
        void* cookie, uint32_t tri_id, const gmio_stl_triangle* tri)
{
    StlMesh_Mesh* mesh = static_cast<StlMesh_Mesh*>(cookie);
    if (tri_id == 0)
        mesh->AddDomain();
    const gmio_vec3f& v1 = tri->v1;
    const gmio_vec3f& v2 = tri->v2;
    const gmio_vec3f& v3 = tri->v3;
    const gmio_vec3f& n = tri->n;
    mesh->AddTriangle(mesh->AddOnlyNewVertex(v1.x, v1.y, v1.z),
                      mesh->AddOnlyNewVertex(v2.x, v2.y, v2.z),
                      mesh->AddOnlyNewVertex(v3.x, v3.y, v3.z),
                      n.x, n.y, n.z);
}

} // namespace internal

gmio_stl_mesh_creator gmio_stl_occmesh_creator(StlMesh_Mesh* mesh)
{
    gmio_stl_mesh_creator creator = {};
    creator.cookie = mesh;
    creator.func_add_triangle = internal::occmesh_add_triangle;
    return creator;
}

gmio_stl_mesh_creator gmio_stl_occmesh_creator(const Handle_StlMesh_Mesh &hnd)
{
    return gmio_stl_occmesh_creator(hnd.operator->());
}

gmio_stl_mesh_occmesh::gmio_stl_mesh_occmesh()
    : m_mesh(NULL),
      m_mesh_domain_count(0),
      m_seq_triangle(NULL),
      m_seq_vertex(NULL)
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

    const StlMesh_MeshTriangle* occ_tri;
    const TColgp_SequenceOfXYZ* occ_vertices;
    if (it->m_mesh_domain_count > 1) {
        const triangle_data& tridata = it->m_vec_triangle_data.at(tri_id);
        occ_tri = tridata.ptr_triangle;
        occ_vertices = tridata.ptr_vec_vertices;
    }
    else {
        occ_tri = it->m_seq_triangle->Value(tri_id + 1).operator->();
        occ_vertices = it->m_seq_vertex;
    }

    int iv1, iv2, iv3;
    double nx, ny, nz;
    occ_tri->GetVertexAndOrientation(iv1, iv2, iv3, nx, ny, nz);
    gmio_stl_occ_copy_xyz(&tri->n, nx, ny, nz);
    gmio_stl_occ_copy_xyz(&tri->v1, occ_vertices->Value(iv1));
    gmio_stl_occ_copy_xyz(&tri->v2, occ_vertices->Value(iv2));
    gmio_stl_occ_copy_xyz(&tri->v3, occ_vertices->Value(iv3));
}

void gmio_stl_mesh_occmesh::init_cache()
{
    if (m_mesh == NULL)
        return;

    // Count triangles
    m_mesh_domain_count = m_mesh != NULL ? m_mesh->NbDomains() : 0;
    for (int dom_id = 1; dom_id <= m_mesh_domain_count; ++dom_id)
        this->triangle_count += m_mesh->NbTriangles(dom_id);

    if (m_mesh_domain_count > 1) {
        // Fill vector of triangle data
        m_vec_triangle_data.reserve(this->triangle_count);
        for (int dom_id = 1; dom_id <= m_mesh_domain_count; ++dom_id) {
            const StlMesh_SequenceOfMeshTriangle& seq_triangles =
                    m_mesh->Triangles(dom_id);
            const TColgp_SequenceOfXYZ& seq_vertices =
                    m_mesh->Vertices(dom_id);
            for (int tri_id = 1; tri_id <= seq_triangles.Length(); ++tri_id) {
                const Handle_StlMesh_MeshTriangle& hnd_occtri =
                        seq_triangles.Value(tri_id);
                struct triangle_data tridata;
                tridata.ptr_triangle = hnd_occtri.operator->();
                tridata.ptr_vec_vertices = &seq_vertices;
                m_vec_triangle_data.push_back(std::move(tridata));
            }
        }
    }
    else {
        m_seq_triangle = &m_mesh->Triangles(1);
        m_seq_vertex = &m_mesh->Vertices(1);
    }
}
