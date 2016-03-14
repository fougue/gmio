/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
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

#include <gmio_support/stl_occ.h>

#include <cstring>
#include <StlMesh_Mesh.hxx>
#include <StlMesh_MeshTriangle.hxx>
#include <StlMesh_SequenceOfMeshTriangle.hxx>
#include <TColgp_SequenceOfXYZ.hxx>

namespace internal {

/* Common */

static StlMesh_Mesh* occMeshPtr(const Handle_StlMesh_Mesh& mesh)
{
    return mesh.operator->();
}

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

static inline void gmio_stl_occ_copy_xyz(
        gmio_vec3f* stl_coords, const gp_XYZ& coords)
{
    stl_coords->x = static_cast<float>(coords.X());
    stl_coords->y = static_cast<float>(coords.Y());
    stl_coords->z = static_cast<float>(coords.Z());
}

static void occmesh_get_triangle(
        const void* cookie, uint32_t tri_id, gmio_stl_triangle* tri)
{
    void* wcookie = const_cast<void*>(cookie);
    gmio_stl_occmesh_iterator* it =
            static_cast<gmio_stl_occmesh_iterator*>(wcookie);

    if (it->move_to_next_tri(tri_id)) {
        const Handle_StlMesh_MeshTriangle& occTri = it->domain_tri(tri_id);
        int idV1, idV2, idV3;
        double xN, yN, zN;
        occTri->GetVertexAndOrientation(idV1, idV2, idV3, xN, yN, zN);
        gmio_vec3f& n = tri->n;
        n.x = static_cast<float>(xN);
        n.y = static_cast<float>(yN);
        n.z = static_cast<float>(zN);

        const TColgp_SequenceOfXYZ& vertices = it->domain_vertices();
        gmio_stl_occ_copy_xyz(&tri->v1, vertices.Value(idV1));
        gmio_stl_occ_copy_xyz(&tri->v2, vertices.Value(idV2));
        gmio_stl_occ_copy_xyz(&tri->v3, vertices.Value(idV3));
    }
}

} // namespace internal

gmio_stl_mesh gmio_stl_occmesh(const gmio_stl_occmesh_iterator& it)
{
    gmio_stl_mesh mesh = {};
    mesh.cookie = &it;
    const int domain_count = it.mesh()->NbDomains();
    for (int dom_id = 1; dom_id <= domain_count; ++dom_id)
        mesh.triangle_count += it.mesh()->NbTriangles(dom_id);
    mesh.func_get_triangle = internal::occmesh_get_triangle;
    return mesh;
}

gmio_stl_mesh_creator gmio_stl_occmesh_creator(StlMesh_Mesh* mesh)
{
    gmio_stl_mesh_creator creator = {};
    creator.cookie = mesh;
    creator.func_add_triangle = internal::occmesh_add_triangle;
    return creator;
}

gmio_stl_mesh_creator gmio_stl_occmesh_creator(const Handle_StlMesh_Mesh &hnd)
{
    return gmio_stl_occmesh_creator(internal::occMeshPtr(hnd));
}

gmio_stl_occmesh_iterator::gmio_stl_occmesh_iterator()
{
    this->init(NULL);
}

gmio_stl_occmesh_iterator::gmio_stl_occmesh_iterator(const StlMesh_Mesh *mesh)
{
    this->init(mesh);
}

gmio_stl_occmesh_iterator::gmio_stl_occmesh_iterator(const Handle_StlMesh_Mesh &hnd)
{
    this->init(internal::occMeshPtr(hnd));
}

void gmio_stl_occmesh_iterator::init(const StlMesh_Mesh* mesh)
{
    m_mesh = mesh;
    m_domain_id = 0;
    m_domain_count = m_mesh != NULL ? m_mesh->NbDomains() : 0;
    m_domain_triangles = NULL;
    m_domain_vertices = NULL;
    m_domain_first_tri_id = 0;
    m_domain_last_tri_id = 0;
    if (m_domain_count > 0)
        this->cache_domain(1);
}

void gmio_stl_occmesh_iterator::cache_domain(int dom_id)
{
    m_domain_id = dom_id;
    m_domain_triangles = &m_mesh->Triangles(dom_id);
    m_domain_vertices = &m_mesh->Vertices(dom_id);
    const int dom_tricnt = m_domain_triangles->Length();
    m_domain_first_tri_id =
            dom_tricnt > 0 ? m_domain_last_tri_id : m_domain_first_tri_id;
    m_domain_last_tri_id +=
            dom_tricnt > 0 ? dom_tricnt - 1 : 0;
}

bool gmio_stl_occmesh_iterator::move_to_next_tri(uint32_t tri_id)
{
    if (tri_id > m_domain_last_tri_id) {
        if (m_domain_id < m_domain_count) {
            ++m_domain_id;
            this->cache_domain(m_domain_id);
            return true;
        }
        return false;
    }
    return true;
}
