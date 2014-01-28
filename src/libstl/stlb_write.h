#ifndef FOUG_DATAX_C_LIBSTL_STLB_WRITE_H
#define FOUG_DATAX_C_LIBSTL_STLB_WRITE_H

#include "stl_global.h"
#include "stlb_triangle.h"
#include "../endian.h"
#include "../transfer.h"

typedef struct
{
  const uint8_t* header; /* May be NULL if empty header */
  uint32_t       triangle_count;
  const void*    cookie;
  void (*get_triangle_func)(const void*, uint32_t, foug_stl_triangle_t*);
  void (*get_attr_byte_count_func)(const void*, uint32_t, uint16_t*); /* Optional : may be NULL */
} foug_stlb_geom_output_t;

/* foug_stlb_write() */
FOUG_DATAX_LIBSTL_EXPORT int foug_stlb_write(const foug_stlb_geom_output_t* geom,
                                             foug_transfer_t*               trsf,
                                             foug_endianness_t              byte_order);

#endif /* FOUG_DATAX_C_LIBSTL_STLB_WRITE_H */
