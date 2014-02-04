#ifndef FOUG_SUPPORT_OCC_LIBSTL_H
#define FOUG_SUPPORT_OCC_LIBSTL_H

extern "C" {
#include "support_global.h"
#include "../libstl/stla_read.h"
#include "../libstl/stla_write.h"
#include "../libstl/stlb_read.h"
#include "../libstl/stlb_write.h"
}
#include <Handle_StlMesh_Mesh.hxx>

class FOUG_LIBSUPPORT_EXPORT foug_OccStlMeshDomain
{
public:
  foug_OccStlMeshDomain(const Handle_StlMesh_Mesh& stlMesh, int domId = 1);

  const Handle_StlMesh_Mesh& mesh() const;
  void setMesh(const Handle_StlMesh_Mesh& stlMesh);

  int domainId() const;
  void setDomainId(int domId);

private:
  Handle_StlMesh_Mesh m_mesh;
  int m_domainId;
};

FOUG_LIBSUPPORT_EXPORT
void foug_stl_occmesh_geom(foug_stl_geom_t* geom,
                           const foug_OccStlMeshDomain& meshCookie);

FOUG_LIBSUPPORT_EXPORT
void foug_stl_occmesh_geom_creator(foug_stl_geom_creator_t* creator,
                                   const Handle_StlMesh_Mesh& mesh);

#endif /* FOUG_SUPPORT_OCC_LIBSTL_H */