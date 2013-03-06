#ifndef FOUG_DATAX_C_LIBSTL_STLB_READ_H
#define FOUG_DATAX_C_LIBSTL_STLB_READ_H

#include "stl_global.h"
#include "stlb_triangle.h"
#include "../transfer.h"

/* foug_stlb_geom_input */
typedef struct foug_stlb_geom_input foug_stlb_geom_input_t;
struct foug_stlb_geom_input
{
  void* cookie;
  void (*process_header_func)       (foug_stlb_geom_input_t*, const uint8_t*);
  void (*begin_triangles_func)      (foug_stlb_geom_input_t*, uint32_t);
  void (*process_next_triangle_func)(foug_stlb_geom_input_t*, const foug_stlb_triangle_t*);
  void (*end_triangles_func)        (foug_stlb_geom_input_t*);
};

/* foug_stlb_read() */
FOUG_DATAX_LIBSTL_EXPORT int foug_stlb_read(foug_stlb_geom_input_t* geom,
                                            foug_transfer_t* trsf);

/* Specific error codes returned by foug_stlb_read() */
#define FOUG_STLB_READ_HEADER_WRONG_SIZE_ERROR   1
#define FOUG_STLB_READ_FACET_COUNT_ERROR         2

#endif /* FOUG_DATAX_C_LIBSTL_STLB_READ_H */
