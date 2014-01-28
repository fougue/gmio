#ifndef FOUG_DATAX_C_LIBSTL_STLA_WRITE_H
#define FOUG_DATAX_C_LIBSTL_STLA_WRITE_H

#include "stl_global.h"
#include "stl_triangle.h"
#include "../transfer.h"

/* Custom geometry expressed with STL ascii interface */
typedef struct foug_stla_geom_output
{
  const char* solid_name; /* May be NULL to generate default name */
  uint32_t    triangle_count;
  const void* cookie;
  void (*get_triangle_func)(const void*, uint32_t, foug_stl_triangle_t*);
} foug_stla_geom_output_t;

/* Write geometry in the STL ascii format */
FOUG_DATAX_LIBSTL_EXPORT int foug_stla_write(foug_stla_geom_output_t* geom,
                                             foug_transfer_t*         trsf,
                                             uint8_t                  real32_prec);

#endif /* FOUG_DATAX_C_LIBSTL_STLA_WRITE_H */
