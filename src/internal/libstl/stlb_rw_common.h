#ifndef FOUG_INTERNAL_STLB_RW_COMMON_H
#define FOUG_INTERNAL_STLB_RW_COMMON_H

#include "../../global.h"
#include "../../endian.h"
#include "../../transfer.h"
#include "../../libstl/stl_triangle.h"

typedef struct
{
  uint32_t facet_count;
  uint32_t i_facet_offset;
  void   (*fix_endian_func)(foug_stl_triangle_t*);
} foug_readwrite_helper;

int foug_stlb_check_params(const foug_transfer_t* trsf,
                           foug_endianness_t byte_order);

#endif /* FOUG_INTERNAL_STLB_RW_COMMON_H */
