#ifndef FOUG_DATAX_C_LIBSTL_TRIANGLE_H
#define FOUG_DATAX_C_LIBSTL_TRIANGLE_H

#include "stl_global.h"

typedef struct foug_stl_coords
{
  foug_real32_t x;
  foug_real32_t y;
  foug_real32_t z;
} foug_stl_coords_t;
#define FOUG_STL_COORDS_RAWSIZE (3 * sizeof(foug_real32_t))

typedef struct foug_stl_triangle
{
  foug_stl_coords_t normal;
  foug_stl_coords_t v1;
  foug_stl_coords_t v2;
  foug_stl_coords_t v3;
} foug_stl_triangle_t;
#define FOUG_STL_TRIANGLE_RAWSIZE (4 * FOUG_STL_COORDS_RAWSIZE)

#endif /* FOUG_DATAX_C_LIBSTL_TRIANGLE_H */
