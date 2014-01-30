#ifndef FOUG_INTERNAL_STL_RW_COMMON_H
#define FOUG_INTERNAL_STL_RW_COMMON_H

#include "../../global.h"
#include "../../endian.h"
#include "../../transfer.h"
#include "../../libstl/stl_geom.h"
#include "../../libstl/stl_triangle.h"

typedef struct
{
  uint32_t facet_count;
  uint32_t i_facet_offset;
  void   (*fix_endian_func)(foug_stl_triangle_t*);
} foug_stlb_readwrite_helper;

foug_bool_t foug_check_transfer(int* error, const foug_transfer_t* trsf);

foug_bool_t foug_stl_check_geom(int* error, const foug_stl_geom_t* geom);

foug_bool_t foug_stlb_check_params(int* error,
                                   const foug_transfer_t* trsf,
                                   foug_endianness_t byte_order);

#endif /* FOUG_INTERNAL_STLB_RW_COMMON_H */
