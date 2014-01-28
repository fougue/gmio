#ifndef FOUG_DATAX_C_LIBSTL_STLB_READ_H
#define FOUG_DATAX_C_LIBSTL_STLB_READ_H

#include "stl_global.h"
#include "stlb_triangle.h"
#include "../endian.h"
#include "../transfer.h"

/* foug_stlb_geom_input */
typedef struct
{
  void* cookie;
  /* All function pointers can be safely set to NULL */
  void (*process_header_func)  (void*, const uint8_t*);
  void (*begin_triangles_func) (void*, uint32_t);
  void (*process_triangle_func)(void*, uint32_t, const foug_stl_triangle_t*, uint16_t);
  void (*end_triangles_func)   (void*);
} foug_stlb_geom_input_t;
typedef void (*foug_stlb_begin_triangles_func_t)(void*, uint32_t);
typedef void (*foug_stlb_process_triangle_func_t)(void*, uint32_t, const foug_stl_triangle_t*, uint16_t);

/* foug_stlb_read() */
FOUG_DATAX_LIBSTL_EXPORT int foug_stlb_read(foug_stlb_geom_input_t* geom,
                                            foug_transfer_t*        trsf,
                                            foug_endianness_t       byte_order);

#endif /* FOUG_DATAX_C_LIBSTL_STLB_READ_H */
