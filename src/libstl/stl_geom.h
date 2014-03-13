#ifndef FOUG_LIBSTL_STL_GEOM_H
#define FOUG_LIBSTL_STL_GEOM_H

#include "stl_global.h"
#include "stl_triangle.h"

/*! Provides an interface for accessing the underlying(hidden) user mesh */
struct foug_stl_geom
{
  /*! Opaque pointer on the user mesh, passed as first argument to hook functions */
  const void* cookie;

  /*! Number of triangles in the mesh */
  uint32_t    triangle_count;

  /*! Pointer on a function that stores the mesh triangle of index \p tri_id into \p triangle */
  void      (*get_triangle_func)(const void* cookie, uint32_t tri_id, foug_stl_triangle_t* triangle);
};

typedef struct foug_stl_geom foug_stl_geom_t;

#endif /* FOUG_LIBSTL_STL_GEOM_H */
