#ifndef GMIO_SUPPORT_OCC_LIBSTL_H
#define GMIO_SUPPORT_OCC_LIBSTL_H

#include "support_global.h"
#include <Handle_StlMesh_Mesh.hxx>
struct gmio_stl_mesh;
struct gmio_stl_mesh_creator;

class GMIO_LIBSUPPORT_EXPORT gmio_OccStlMeshDomain
{
public:
  gmio_OccStlMeshDomain(const Handle_StlMesh_Mesh& stlMesh, int domId = 1);

  const Handle_StlMesh_Mesh& mesh() const;
  void setMesh(const Handle_StlMesh_Mesh& stlMesh);

  int domainId() const;
  void setDomainId(int domId);

private:
  Handle_StlMesh_Mesh m_mesh;
  int m_domainId;
};

GMIO_LIBSUPPORT_EXPORT
void gmio_stl_occmesh(gmio_stl_mesh* mesh, const gmio_OccStlMeshDomain& meshCookie);

GMIO_LIBSUPPORT_EXPORT
void gmio_stl_occmesh_creator(gmio_stl_mesh_creator* creator, const Handle_StlMesh_Mesh& mesh);

#endif /* GMIO_SUPPORT_OCC_LIBSTL_H */
