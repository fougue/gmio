#ifndef FOUG_DATAX_C_LIBSTL_STLA_WRITE_H
#define FOUG_DATAX_C_LIBSTL_STLA_WRITE_H

#include "stl_global.h"
#include "stl_triangle.h"
#include "../transfer.h"

/* foug_stla_geom_output */
typedef struct foug_stla_geom_output foug_stla_geom_output_t;
struct foug_stla_geom_output
{
  void*  cookie;
  size_t (*get_solid_count_func)   (foug_stla_geom_output_t*); /* Optional */
  void   (*get_solid_name)         (foug_stla_geom_output_t*, size_t, char*); /* Optional */
  size_t (*get_triangle_count_func)(foug_stla_geom_output_t*, size_t);
  void   (*get_triangle_func)      (foug_stla_geom_output_t*, size_t, size_t, foug_stl_triangle_t*);
};

/* foug_stla_write() */
FOUG_DATAX_LIBSTL_EXPORT int foug_stla_write(foug_stla_geom_output_t* geom,
                                             foug_transfer_t*         trsf,
                                             uint8_t                  real32_precision);

/* Specific error codes returned by foug_stla_write() */
#define FOUG_STLA_WRITE_NULL_GET_TRIANGLE_COUNT_FUNC 1
#define FOUG_STLA_WRITE_NULL_GET_TRIANGLE_FUNC       2
#define FOUG_STLA_WRITE_INVALID_REAL32_PRECISION     3

#endif /* FOUG_DATAX_C_LIBSTL_STLA_WRITE_H */
