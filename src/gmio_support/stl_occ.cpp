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

static void occmesh_add_triangle(void* cookie,
                                 uint32_t tri_id,
                                 const gmio_stl_triangle_t* tri)
{
    StlMesh_Mesh* mesh = static_cast<StlMesh_Mesh*>(cookie);
    if (tri_id == 0)
        mesh->AddDomain();
    mesh->AddTriangle(mesh->AddOnlyNewVertex(tri->v1.x, tri->v1.y, tri->v1.z),
                      mesh->AddOnlyNewVertex(tri->v2.x, tri->v2.y, tri->v2.z),
                      mesh->AddOnlyNewVertex(tri->v3.x, tri->v3.y, tri->v3.z),
                      tri->normal.x, tri->normal.y, tri->normal.z);
}

static void occmesh_get_triangle(
        const void* cookie, uint32_t tri_id, gmio_stl_triangle_t* triangle)
{
    const gmio_occ_stl_mesh_domain_t* mesh_domain =
            static_cast<const gmio_occ_stl_mesh_domain_t*>(cookie);
    const StlMesh_SequenceOfMeshTriangle& occTriangles =
            mesh_domain->mesh->Triangles(mesh_domain->domain_id);
    const Handle_StlMesh_MeshTriangle& occTri =
            occTriangles.Value(tri_id + 1);
    int v1;
    int v2;
    int v3;
    double xN;
    double yN;
    double zN;
    occTri->GetVertexAndOrientation(v1, v2, v3, xN, yN, zN);
    triangle->normal.x = float(xN);
    triangle->normal.y = float(yN);
    triangle->normal.z = float(zN);

    const TColgp_SequenceOfXYZ& vertices =
            mesh_domain->mesh->Vertices(mesh_domain->domain_id);
    const gp_XYZ& coordsV1 = vertices.Value(v1);
    const gp_XYZ& coordsV2 = vertices.Value(v2);
    const gp_XYZ& coordsV3 = vertices.Value(v3);
    triangle->v1.x = float(coordsV1.X());
    triangle->v2.x = float(coordsV2.X());
    triangle->v3.x = float(coordsV3.X());

    triangle->v1.y = float(coordsV1.Y());
    triangle->v2.y = float(coordsV2.Y());
    triangle->v3.y = float(coordsV3.Y());

    triangle->v1.z = float(coordsV1.Z());
    triangle->v2.z = float(coordsV2.Z());
    triangle->v3.z = float(coordsV3.Z());
}

} // namespace internal

gmio_stl_mesh_t gmio_stl_occmesh(const gmio_occ_stl_mesh_domain_t* mesh_domain)
{
    gmio_stl_mesh_t mesh = { 0 };
    mesh.cookie = mesh_domain;
    if (mesh_domain != NULL && mesh_domain->mesh != NULL) {
        mesh.triangle_count =
                mesh_domain->mesh->NbTriangles(mesh_domain->domain_id);
    }
    mesh.func_get_triangle = internal::occmesh_get_triangle;
    return mesh;
}

gmio_stl_mesh_creator_t gmio_stl_occmesh_creator(StlMesh_Mesh* mesh)
{
    gmio_stl_mesh_creator_t creator = { 0 };
    creator.cookie = mesh;
    creator.func_add_triangle = internal::occmesh_add_triangle;
    return creator;
}

gmio_stl_mesh_creator_t gmio_stl_hnd_occmesh_creator(const Handle_StlMesh_Mesh &hnd)
{
    return gmio_stl_occmesh_creator(internal::occMeshPtr(hnd));
}

gmio_occ_stl_mesh_domain::gmio_occ_stl_mesh_domain()
    : mesh(NULL),
      domain_id(0)
{
}

gmio_occ_stl_mesh_domain::gmio_occ_stl_mesh_domain(
        const StlMesh_Mesh *msh, int dom_id)
    : mesh(msh),
      domain_id(dom_id)

{
}

gmio_occ_stl_mesh_domain::gmio_occ_stl_mesh_domain(
        const Handle_StlMesh_Mesh &hndMesh, int dom_id)
    : mesh(internal::occMeshPtr(hndMesh)),
      domain_id(dom_id)
{
}
