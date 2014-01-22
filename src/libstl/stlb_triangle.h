#ifndef FOUG_DATAX_C_LIBSTL_STLB_TRIANGLE_H
#define FOUG_DATAX_C_LIBSTL_STLB_TRIANGLE_H

#include "stl_triangle.h"

typedef struct foug_stlb_triangle
{
  foug_stl_triangle_t data;
  uint16_t            attribute_byte_count;
} foug_stlb_triangle_t;
#define FOUG_STLB_TRIANGLE_RAWSIZE (FOUG_STL_TRIANGLE_RAWSIZE + sizeof(uint16_t))

#endif /* FOUG_DATAX_C_LIBSTL_STLB_TRIANGLE_H */
