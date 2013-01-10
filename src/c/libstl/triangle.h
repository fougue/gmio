#ifndef FOUG_C_LIBSTL_TRIANGLE_H
#define FOUG_C_LIBSTL_TRIANGLE_H

#include "../foug_global.h"

typedef struct foug_stl_coords
{
  foug_real32 x;
  foug_real32 y;
  foug_real32 z;
} foug_stl_coords_t;

typedef struct foug_stl_triangle
{
  foug_stl_coords_t normal;
  foug_stl_coords_t v1;
  foug_stl_coords_t v2;
  foug_stl_coords_t v3;
} foug_stl_triangle_t;

#endif /* FOUG_C_LIBSTL_TRIANGLE_H */
