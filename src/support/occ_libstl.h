#ifndef FOUG_SUPPORT_OCC_LIBSTL_H
#define FOUG_SUPPORT_OCC_LIBSTL_H

extern "C" {
#include "support_global.h"
#include "../libstl/stla_read.h"
#include "../libstl/stla_write.h"
#include "../libstl/stlb_read.h"
#include "../libstl/stlb_write.h"
}

class Handle_StlMesh_Mesh;

/* ASCII STL */

FOUG_LIBSUPPORT_EXPORT
void foug_stla_geom_input_set_occmesh(foug_stla_geom_input_t* input, Handle_StlMesh_Mesh* mesh);

FOUG_LIBSUPPORT_EXPORT
void foug_stla_geom_output_set_occmesh(foug_stla_geom_output_t* output, Handle_StlMesh_Mesh* mesh);

/* Binary STL */

FOUG_LIBSUPPORT_EXPORT
void foug_stlb_geom_input_set_occmesh(foug_stlb_geom_input_t* input, Handle_StlMesh_Mesh* mesh);

FOUG_LIBSUPPORT_EXPORT
void foug_stlb_geom_output_set_occmesh(foug_stlb_geom_output_t* output, Handle_StlMesh_Mesh* mesh);

#endif /* FOUG_SUPPORT_OCC_LIBSTL_H */
