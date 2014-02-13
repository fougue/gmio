#ifndef FOUG_LIBSTL_STL_GEOM_H
#define FOUG_LIBSTL_STL_GEOM_H

#include "stl_global.h"
#include "stl_triangle.h"

struct foug_stl_geom
{
  const void* cookie;
  uint32_t    triangle_count;
  void      (*get_triangle_func)(const void*, uint32_t, foug_stl_triangle_t*);
};

typedef struct foug_stl_geom foug_stl_geom_t;

#endif /* FOUG_LIBSTL_STL_GEOM_H */
