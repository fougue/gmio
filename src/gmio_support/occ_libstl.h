#ifndef GMIO_SUPPORT_OCC_LIBSTL_H
#define GMIO_SUPPORT_OCC_LIBSTL_H

#include "support_global.h"
#include <Handle_StlMesh_Mesh.hxx>
struct gmio_stl_mesh;
struct gmio_stl_mesh_creator;

/*! \brief Domain in a StlMesh_Mesh object
 *
 *  The domain is indicated with its index within the STL mesh
 */
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

/*! \brief Initializes \p mesh so it maps to a domain in StlMesh_Mesh
 *
 *  \c mesh->cookie will point to \p meshCookie
 */
GMIO_LIBSUPPORT_EXPORT
void gmio_stl_occmesh(gmio_stl_mesh* mesh, const gmio_OccStlMeshDomain& meshCookie);

/*! \brief Initializes \p creator to build a new domain in a StlMesh_Mesh object
 *
 *  \c creator->cookie will point to the internal data(ie. StlMesh_Mesh*) of handle \p mesh
 */
GMIO_LIBSUPPORT_EXPORT
void gmio_stl_occmesh_creator(gmio_stl_mesh_creator* creator, const Handle_StlMesh_Mesh& mesh);

#endif /* GMIO_SUPPORT_OCC_LIBSTL_H */
