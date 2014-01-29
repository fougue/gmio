#ifndef FOUG_LIBSTL_STL_GEOM_CREATOR_H
#define FOUG_LIBSTL_STL_GEOM_CREATOR_H

#include "stl_global.h"
#include "stl_triangle.h"

typedef struct
{
  void* cookie;

  void (*ascii_begin_solid_func)(void*, const char*); /* Optional */
  void (*binary_begin_solid_func)(void*, uint32_t, const uint8_t*); /* Optional */

  void (*add_triangle_func)(void*, uint32_t, const foug_stl_triangle_t*); /* Optional */

  void (*end_solid_func)       (void*); /* Optional */
} foug_stl_geom_creator_t;

#endif /* FOUG_LIBSTL_STL_GEOM_CREATOR_H */
