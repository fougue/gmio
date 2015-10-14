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
        void* cookie, uint32_t tri_id, const gmio_stl_triangle_t* tri)
{
    StlMesh_Mesh* mesh = static_cast<StlMesh_Mesh*>(cookie);
    if (tri_id == 0)
        mesh->AddDomain();
    const gmio_stl_coords& v1 = tri->v1;
    const gmio_stl_coords& v2 = tri->v2;
    const gmio_stl_coords& v3 = tri->v3;
    const gmio_stl_coords& n = tri->normal;
    mesh->AddTriangle(mesh->AddOnlyNewVertex(v1.x, v1.y, v1.z),
                      mesh->AddOnlyNewVertex(v2.x, v2.y, v2.z),
                      mesh->AddOnlyNewVertex(v3.x, v3.y, v3.z),
                      n.x, n.y, n.z);
}

static void occmesh_get_triangle(
        const void* cookie, uint32_t tri_id, gmio_stl_triangle_t* tri)
{
    const gmio_occ_stl_mesh_domain_t* mesh_domain =
            static_cast<const gmio_occ_stl_mesh_domain_t*>(cookie);
    const Handle_StlMesh_MeshTriangle& occTri =
            mesh_domain->triangles()->Value(tri_id + 1);
    int idV1;
    int idV2;
    int idV3;
    double xN;
    double yN;
    double zN;
    occTri->GetVertexAndOrientation(idV1, idV2, idV3, xN, yN, zN);
    gmio_stl_coords& n = tri->normal;
    n.x = static_cast<float>(xN);
    n.y = static_cast<float>(yN);
    n.z = static_cast<float>(zN);

    const TColgp_SequenceOfXYZ& vertices = *mesh_domain->vertices();
    const gp_XYZ& coordsV1 = vertices.Value(idV1);
    gmio_stl_coords& v1 = tri->v1;
    v1.x = static_cast<float>(coordsV1.X());
    v1.y = static_cast<float>(coordsV1.Y());
    v1.z = static_cast<float>(coordsV1.Z());

    const gp_XYZ& coordsV2 = vertices.Value(idV2);
    gmio_stl_coords& v2 = tri->v2;
    v2.x = static_cast<float>(coordsV2.X());
    v2.y = static_cast<float>(coordsV2.Y());
    v2.z = static_cast<float>(coordsV2.Z());

    const gp_XYZ& coordsV3 = vertices.Value(idV3);
    gmio_stl_coords& v3 = tri->v3;
    v3.x = static_cast<float>(coordsV3.X());
    v3.y = static_cast<float>(coordsV3.Y());
    v3.z = static_cast<float>(coordsV3.Z());
}

} // namespace internal

gmio_stl_mesh_t gmio_stl_occmesh(const gmio_occ_stl_mesh_domain_t* mesh_domain)
{
    gmio_stl_mesh_t mesh = {0};
    mesh.cookie = mesh_domain;
    if (mesh_domain != NULL && mesh_domain->mesh() != NULL) {
        mesh.triangle_count =
                mesh_domain->mesh()->NbTriangles(mesh_domain->domain_id());
    }
    mesh.func_get_triangle = internal::occmesh_get_triangle;
    return mesh;
}

gmio_stl_mesh_creator_t gmio_stl_occmesh_creator(StlMesh_Mesh* mesh)
{
    gmio_stl_mesh_creator_t creator = {0};
    creator.cookie = mesh;
    creator.func_add_triangle = internal::occmesh_add_triangle;
    return creator;
}

gmio_stl_mesh_creator_t gmio_stl_hnd_occmesh_creator(const Handle_StlMesh_Mesh &hnd)
{
    return gmio_stl_occmesh_creator(internal::occMeshPtr(hnd));
}

gmio_occ_stl_mesh_domain::gmio_occ_stl_mesh_domain()
    : m_mesh(NULL),
      m_domain_id(0),
      m_triangles(NULL),
      m_vertices(NULL)
{
}

gmio_occ_stl_mesh_domain::gmio_occ_stl_mesh_domain(
        const StlMesh_Mesh *msh, int dom_id)
    : m_mesh(msh),
      m_domain_id(dom_id),
      m_triangles(&msh->Triangles(dom_id)),
      m_vertices(&msh->Vertices(dom_id))
{
}

gmio_occ_stl_mesh_domain::gmio_occ_stl_mesh_domain(
        const Handle_StlMesh_Mesh &hnd, int dom_id)
    : m_mesh(internal::occMeshPtr(hnd)),
      m_domain_id(dom_id),
      m_triangles(&m_mesh->Triangles(dom_id)),
      m_vertices(&m_mesh->Vertices(dom_id))
{
}
