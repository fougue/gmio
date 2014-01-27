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

struct FOUG_LIBSUPPORT_EXPORT foug_OccStlMeshDomain
{
  foug_OccStlMeshDomain(const Handle_StlMesh_Mesh& stlMesh, int stlDomainId = 1);
  Handle_StlMesh_Mesh mesh;
  int domainId;
};

/* ASCII STL */

FOUG_LIBSUPPORT_EXPORT
void foug_stla_geom_input_set_occmesh(foug_stla_geom_input_t* input,
                                      const Handle_StlMesh_Mesh& mesh);

FOUG_LIBSUPPORT_EXPORT
void foug_stla_geom_output_set_occmesh(foug_stla_geom_output_t* output,
                                       const foug_OccStlMeshDomain& meshCookie);

/* Binary STL */

FOUG_LIBSUPPORT_EXPORT
void foug_stlb_geom_input_set_occmesh(foug_stlb_geom_input_t* input,
                                      const Handle_StlMesh_Mesh& mesh);

FOUG_LIBSUPPORT_EXPORT
void foug_stlb_geom_output_set_occmesh(foug_stlb_geom_output_t* output,
                                       const foug_OccStlMeshDomain& meshCookie);

#endif /* FOUG_SUPPORT_OCC_LIBSTL_H */
